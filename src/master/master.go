package master

import (
	"fmt"
	"html/template"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"regexp"
	"strings"
	"time"

	"github.com/gorilla/mux"
	"github.com/luchev/dtf/consts"
	"github.com/luchev/dtf/structs/iplimiter"
	"github.com/luchev/dtf/structs/response"
	"github.com/luchev/dtf/structs/test"
	"github.com/luchev/dtf/structs/testsuite"
	"github.com/luchev/dtf/util"
	"golang.org/x/time/rate"
)

var requestLimit = 2
var requestRefreshTime = rate.Every(60 * time.Minute)
var limiter = iplimiter.NewIPRateLimiter(requestRefreshTime, requestLimit)

func SetupRoutes(port int) {
	log.Printf("Initializing Master server routes")
	log.Printf("Master service started on port %d", port)

	router := mux.NewRouter()
	router.HandleFunc("/test", handleTest)
	router.HandleFunc("/", handleIndex)
	srv := http.Server{
		Handler:      router,
		Addr:         fmt.Sprintf(":%d", port),
		WriteTimeout: 150 * time.Second,
		ReadTimeout:  300 * time.Second,
	}

	log.Fatal(srv.ListenAndServe())
}

func handleTest(w http.ResponseWriter, r *http.Request) {
	limiter := limiter.GetLimiter(r.RemoteAddr)
	if !limiter.Allow() {
		http.Error(w, http.StatusText(http.StatusTooManyRequests), http.StatusTooManyRequests)
		return
	}

	response := response.Response{
		PageTitle:       "Invalid request",
		StudentIdentity: make(map[string]string),
	}

	defer func() {
		tmpl, _ := template.New("result.html").
			Funcs(template.FuncMap{"escapeNewLineHTML": util.EscapeNewLineHTML}).
			ParseFiles("templates/result.html")
		tmpl.Execute(w, response)
	}()

	// Get file from form
	r.ParseMultipartForm(10 << 20) // 10 MB files
	configName := r.FormValue("config")
	tempDir, fileName, err := util.RetrieveFile(r, "srcFile")
	if err != nil {
		response.Errors = append(response.Errors, test.Error{Name: "Failed upload", Details: err.Error()})
		return
	}
	defer os.RemoveAll(tempDir)
	response.PageTitle = fileName

	config, err := testsuite.UnmarshalConfig(configName, filepath.Join(tempDir, fileName))
	if err != nil {
		response.Errors = append(response.Errors, test.Error{Name: "Invalid config"})
	}

	testResult, testErr := testsuite.TestProject(filepath.Join(tempDir, fileName), config)
	if testErr == nil {
		response.Tasks = testResult.Results
	} else {
		response.Errors = append(response.Errors, *testErr)
	}

	studentIdRegex := regexp.MustCompile(config.StudentIdRegex)
	match := studentIdRegex.FindStringSubmatch(fileName)
	for i, name := range studentIdRegex.SubexpNames() {
		if i != 0 && name != "" {
			response.StudentIdentity[name] = match[i]
		}
	}
}

type IndexData struct {
	Port    int
	Configs []struct {
		Name   string
		Config string
	}
}

func handleIndex(w http.ResponseWriter, r *http.Request) {
	tmpl, _ := template.New("index.html").ParseFiles("templates/index.html")
	data := IndexData{Port: consts.Flags.Port}
	for _, config := range consts.Configs {
		data.Configs = append(data.Configs, struct {
			Name   string
			Config string
		}{Name: strings.TrimPrefix(filepath.Dir(config), consts.ConfigDir), Config: config})
	}
	tmpl.Execute(w, data)
}
