package local

import (
	"encoding/json"
	"fmt"
	"log"
	"path/filepath"
	"regexp"
	"strings"

	"github.com/luchev/dtf/consts"
	"github.com/luchev/dtf/structs/task"
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

	tempDir, fileName, err := util.RetrieveLocalFile(projectPath)
	if err != nil {
		log.Fatal(err)
	}
	// defer os.RemoveAll(tempDir)

	varMap := make(map[string]string)
	varMap["${PROJECT_DIR}"], _ = filepath.Abs(tempDir)
	varMap["${CONFIG_DIR}"], _ = filepath.Abs(filepath.Dir(configName))
	err = util.UnmarshalYamlFile(configName, &suite, varMap)
	if err != nil {
		log.Fatal(err)
	}

	studentId := filepath.Base(projectPath)
	studentIdRegexFind := regexp.MustCompile(suite.StudentIdRegex).FindSubmatch([]byte(projectPath))
	if len(studentIdRegexFind) == 2 {
		studentId = string(studentIdRegexFind[1])
	}

	_, err = util.Unzip(filepath.Join(tempDir, fileName), tempDir)
	if err != nil {
		log.Fatalf("Error extracting file %s: %s", filepath.Join(tempDir, fileName), err.Error())
	}

	out, err := util.ExecuteScript(suite.InitScript)
	if err != nil {
		log.Fatal(err, out)
	}

	taskResults := make([]task.TaskResult, 0)
	for _, suiteTask := range suite.Tasks {
		result := task.TaskResult{suiteTask.Name, true, "", nil, nil, 0}
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
