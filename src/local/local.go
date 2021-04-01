package local

import (
	"encoding/json"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"regexp"
	"strings"

	"github.com/luchev/dtf/consts"
	"github.com/luchev/dtf/structs/task"
	"github.com/luchev/dtf/util"
)

type SuiteConfig struct {
	InitScript            string      `yaml:"initScript"`
	Tasks                 []task.Task `yaml:"tasks"`
	StudentIdRegex        string      `yaml:"studentIdRegex"`
	StudentIdOutputFormat string      `yaml:"studentIdOutputFormat"`
}

type TestSuiteResult struct {
	StudentId map[string]string
	Results   []task.TaskResult
}

func TestProject(projectPath string, configName string) {
	suiteConfig := SuiteConfig{}

	tempDir, fileName, err := util.RetrieveLocalFile(projectPath)
	if err != nil {
		log.Fatal(err)
	}
	defer os.RemoveAll(tempDir)

	varMap := make(map[string]string)
	varMap["${PROJECT_DIR}"], _ = filepath.Abs(tempDir)
	varMap["${CONFIG_DIR}"], _ = filepath.Abs(filepath.Dir(configName))
	err = util.UnmarshalYamlFile(configName, &suiteConfig, varMap)
	if err != nil {
		log.Fatal(err)
	}

	studentId := filepath.Base(projectPath)
	studentIdRegex := regexp.MustCompile(suiteConfig.StudentIdRegex)
	match := studentIdRegex.FindStringSubmatch(studentId)
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

	suiteResult := TestSuiteResult{StudentId: studentIdMap, Results: taskResults}

	outputTestSuiteResult(suiteResult, suiteConfig)
}

func outputTestSuiteResult(result TestSuiteResult, config SuiteConfig) {
	if consts.Flags.OutFormat == "json" {
		fmt.Println(outputJson(result, config))
	} else if consts.Flags.OutFormat == "csv" {
		fmt.Println(outputCsv(result, config))
	} else {
		fmt.Println("Wrong output type, specify json or csv")
	}
}

func outputJson(result TestSuiteResult, config SuiteConfig) string {
	out, _ := json.MarshalIndent(result, "", "    ")
	return string(out)
}

type CsvResult struct {
	memory float64
	total  float64
}

func outputCsv(result TestSuiteResult, config SuiteConfig) string {
	resMap := make(map[string]CsvResult)
	for _, res := range result.Results {
		if len(res.Tests) > 0 {
			memoryPoints := res.Tests[len(res.Tests)-1].Points
			remainingPoints := res.Points - memoryPoints
			resMap[res.Name] = CsvResult{memoryPoints, remainingPoints}
		}
	}

	csvOutput := make([]string, 0)
	for _, t := range config.Tasks {
		res, ok := resMap[t.Name]
		if ok {
			csvOutput = append(csvOutput,
				fmt.Sprintf("%.2f", res.memory), fmt.Sprintf("%.2f", res.total))
		} else {
			csvOutput = append(csvOutput, "0", "0")
		}
	}

	id := strings.Split(config.StudentIdOutputFormat, ",")
	for index, identifier := range id {
		value, ok := result.StudentId[identifier]
		if ok {
			id[index] = value
		} else {
			id[index] = "UNDEFINED"
		}
	}

	return fmt.Sprintf("%s,%s", strings.Join(id, ","), strings.Join(csvOutput, ","))
}
