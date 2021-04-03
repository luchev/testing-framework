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
	"github.com/luchev/dtf/structs/response"
	"github.com/luchev/dtf/structs/task"
	"github.com/luchev/dtf/structs/test"
	"github.com/luchev/dtf/structs/testsuite"
	"github.com/luchev/dtf/util"
)

func SetupRoutes(port int) {
	log.Printf("Initializing Master server routes")
	log.Printf("Master service started on http://127.0.0.1:%d", port)

	router := mux.NewRouter()
	router.HandleFunc("/test", handleMasterTest)
	router.HandleFunc("/", handleMasterIndex)
	srv := http.Server{
		Handler:      router,
		Addr:         fmt.Sprintf(":%d", port),
		WriteTimeout: 150 * time.Second,
		ReadTimeout:  300 * time.Second,
	}

	log.Fatal(srv.ListenAndServe())
}

func handleMasterTest(w http.ResponseWriter, r *http.Request) {
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

	// Extract file
	localArchiveFilePath := tempDir + "/" + fileName
	_, err = util.Unzip(localArchiveFilePath, tempDir)
	if err != nil {
		response.Errors = append(response.Errors, test.Error{Name: "Failed extract", Details: err.Error()})
		return
	}
	defer os.RemoveAll(tempDir)

	testResult, err := TestProject(tempDir, fileName, config)
	if err != nil {
		response.Errors = append(response.Errors, test.Error{Name: "Failed to run tetst", Details: err.Error()})
		return
	}

	response.Tasks = testResult.Results
}

func TestProject(tempDir string, fileName string, configName string) (testsuite.TestSuiteResult, error) {
	suiteConfig := testsuite.SuiteConfig{}

	varMap := make(map[string]string)
	varMap["${PROJECT_DIR}"], _ = filepath.Abs(tempDir)
	varMap["${CONFIG_DIR}"], _ = filepath.Abs(filepath.Dir(configName))
	err := util.UnmarshalYamlFile(configName, &suiteConfig, varMap)
	if err != nil {
		return testsuite.TestSuiteResult{}, err
	}

	studentIdRegex := regexp.MustCompile(suiteConfig.StudentIdRegex)
	match := studentIdRegex.FindStringSubmatch(fileName)
	studentIdMap := make(map[string]string)
	for i, name := range studentIdRegex.SubexpNames() {
		if i != 0 && name != "" {
			studentIdMap[name] = match[i]
		}
	}

	_, err = util.Unzip(filepath.Join(tempDir, fileName), tempDir)
	if err != nil {
		log.Fatalf("Error extracting file %s: %s", filepath.Join(tempDir, fileName), err.Error())
	}

	out, err := util.ExecuteScript(suiteConfig.InitScript)
	if err != nil {
		log.Fatal(err, out)
	}

	taskResults := make([]task.TaskResult, 0)
	for _, suiteTask := range suiteConfig.Tasks {
		result := task.TaskResult{Name: suiteTask.Name, PassingBuild: true, BuildMessage: "", Errors: nil, Tests: nil, Points: 0}
		out, err := util.ExecuteScript(suiteTask.InitScript)
		if err != nil {
			result.PassingBuild = false
			result.BuildMessage = out
			taskResults = append(taskResults, result)
			continue
		}

		testResults := suiteTask.RunTestScript()
		result.Tests = append(result.Tests, testResults...)
		result.Tests = append(result.Tests, suiteTask.MemoryLeakTest())
		for _, t := range result.Tests {
			result.Points += t.Points
		}
		taskResults = append(taskResults, result)
	}

	return testsuite.TestSuiteResult{StudentId: studentIdMap, Results: taskResults}, nil
}

type IndexData struct {
	Port    int
	Configs []struct {
		Name   string
		Config string
	}
}

func handleMasterIndex(w http.ResponseWriter, r *http.Request) {
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
