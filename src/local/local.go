package local

import (
	"encoding/json"
	"fmt"
	"log"
	"path/filepath"
	"regexp"
	"strings"

	"github.com/luchev/dtf/consts"
	"github.com/luchev/dtf/structs/error"
	"github.com/luchev/dtf/structs/task"
	"github.com/luchev/dtf/structs/test"
	"github.com/luchev/dtf/util"
)

type SuiteConfig struct {
	InitScript     string      `yaml:"initScript"`
	Tasks          []task.Task `yaml:"tasks"`
	StudentIdRegex string      `yaml:"studentIdRegex"`
}

type TestSuiteResult struct {
	StudentId string
	Results   []task.TaskResult
}

func TestProject(projectPath string, configName string) {
	suite := SuiteConfig{}
	err := util.UnmarshalYamlFile(configName, &suite)
	if err != nil {
		log.Fatal(err)
	}

	tempDir, fileName, err := util.RetrieveLocalFile(projectPath)
	if err != nil {
		log.Fatal(err)
	}
	// defer os.RemoveAll(tempDir)

	studentId := filepath.Base(projectPath)
	studentIdRegexFind := regexp.MustCompile(suite.StudentIdRegex).FindSubmatch([]byte(projectPath))
	if len(studentIdRegexFind) == 2 {
		studentId = string(studentIdRegexFind[1])
	}

	_, err = util.Unzip(filepath.Join(tempDir, fileName), tempDir)
	if err != nil {
		log.Fatalf("Error extracting file %s: %s", filepath.Join(tempDir, fileName), err.Error())
	}

	for _, variable := range consts.Settings.Vars {
		if variable == "PROJECT_DIR" {
			suite.InitScript = strings.ReplaceAll(suite.InitScript, "${PROJECT_DIR}", tempDir)
		}
		if variable == "CONFIG_DIR" {
			suite.InitScript = strings.ReplaceAll(suite.InitScript, "${CONFIG_DIR}", filepath.Dir(configName))
		}
	}

	for index := 0; index < len(suite.Tasks); index += 1 {
		for _, variable := range consts.Settings.Vars {
			if variable == "PROJECT_DIR" {
				suite.Tasks[index].TestScript = strings.ReplaceAll(suite.Tasks[index].TestScript, "${"+variable+"}", tempDir)
				suite.Tasks[index].MemoryScript = strings.ReplaceAll(suite.Tasks[index].MemoryScript, "${"+variable+"}", tempDir)
			}
			if variable == "CONFIG_DIR" {
				suite.Tasks[index].TestScript = strings.ReplaceAll(suite.Tasks[index].TestScript, "${"+variable+"}", filepath.Dir(configName))
				suite.Tasks[index].MemoryScript = strings.ReplaceAll(suite.Tasks[index].MemoryScript, "${"+variable+"}", filepath.Dir(configName))
			}
		}
	}

	out, err := util.ExecuteScript(suite.InitScript)
	if err != nil {
		log.Fatal(err, out)
	}

	taskResults := make([]task.TaskResult, 0)
	for _, suiteTask := range suite.Tasks {
		result := suiteTask.RunTestScript()
		result.Name = suiteTask.Name
		if !suiteTask.HasMemoryLeak() && result.PassingBuild {
			result.Points += suiteTask.MemoryPoints
			result.Tests = append(result.Tests, test.TestResult{
				Name:    "Memory leak test",
				Passing: true,
				Err:     "",
				Points:  suiteTask.MemoryPoints,
			})
		} else {
			result.Errors = append(result.Errors, error.Error{Name: "Memory leak detected", Details: ""})
			result.Tests = append(result.Tests, test.TestResult{
				Name:    "Memory leak test",
				Passing: false,
				Err:     "",
				Points:  0,
			})
		}

		taskResults = append(taskResults, result)
	}

	suiteResult := TestSuiteResult{StudentId: studentId, Results: taskResults}

	if consts.Flags.OutFormat == "json" {
		out, _ := json.MarshalIndent(suiteResult, "", "    ")
		fmt.Printf("%s", string(out))
	} else if consts.Flags.OutFormat == "csv" {
		points := make([]string, 0)
		for _, t := range suiteResult.Results {
			points = append(points, fmt.Sprintf("%.2f", t.Points))
		}
		fmt.Printf("%s,%s\n", suiteResult.StudentId, strings.Join(points, ","))
	} else {
		fmt.Println("Wrong output type, specify json or csv")
	}
}
