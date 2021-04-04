package master

import (
	"fmt"
	"html/template"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"strings"
	"time"

	"github.com/gorilla/mux"
	"github.com/luchev/dtf/consts"
	"github.com/luchev/dtf/structs/response"
	"github.com/luchev/dtf/structs/test"
	"github.com/luchev/dtf/structs/testsuite"
	"github.com/luchev/dtf/util"
)

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
	response := response.Response{PageTitle: "Invalid request", Tasks: nil, Errors: nil}
	defer func() {
		tmpl, _ := template.New("result.html").
			Funcs(template.FuncMap{"escapeNewLineHTML": util.EscapeNewLineHTML}).
			ParseFiles("templates/result.html")
		tmpl.Execute(w, response)
	}()

	// Get file from form
	r.ParseMultipartForm(10 << 20) // 10 MB files
	config := r.FormValue("config")
	tempDir, fileName, err := util.RetrieveFile(r, "srcFile")
	if err != nil {
		response.Errors = append(response.Errors, test.Error{Name: "Failed upload", Details: err.Error()})
		return
	}
	response.PageTitle = fileName
	defer os.RemoveAll(tempDir)

	testResult, testErr := testsuite.TestProject(filepath.Join(tempDir, fileName), config)
	if testErr != nil {
		response.Errors = append(response.Errors, *testErr)
		return
	}

	// studentIdRegex := regexp.MustCompile(suiteConfig.StudentIdRegex)
	// match := studentIdRegex.FindStringSubmatch(fileName)
	// studentIdMap := make(map[string]string)
	// for i, name := range studentIdRegex.SubexpNames() {
	// 	if i != 0 && name != "" {
	// 		studentIdMap[name] = match[i]
	// 	}
	// }

	response.Tasks = testResult.Results
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
