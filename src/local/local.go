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
	"github.com/luchev/dtf/structs/testsuite"
	"github.com/luchev/dtf/util"
)

type TestSuiteResult struct {
	StudentId map[string]string
	Results   []task.Result
}

func TestProject(projectPath string, configName string) {
	tempDir, fileName, err := util.RetrieveLocalFile(projectPath)
	if err != nil {
		log.Fatal(err)
	}
	defer os.RemoveAll(tempDir)

	config, err := testsuite.UnmarshalConfig(configName, filepath.Join(tempDir, fileName))
	if err != nil {
		log.Fatalf("Invalid config %s: %s", configName, err)
	}

	studentIdMap := make(map[string]string)
	studentIdRegex := regexp.MustCompile(config.StudentIdRegex)
	match := studentIdRegex.FindStringSubmatch(fileName)
	if len(match) == 0 {
		log.Fatalf("Incorrect archive name %s", fileName)
	} else {
		for i, name := range studentIdRegex.SubexpNames() {
			if i != 0 && name != "" {
				studentIdMap[name] = match[i]
			}
		}
	}

	testResult, testErr := testsuite.TestProject(filepath.Join(tempDir, fileName), config)
	if testErr != nil {
		log.Fatal("Error running tests: ", testErr.Name)
	}

	suiteResult := TestSuiteResult{StudentId: studentIdMap, Results: testResult.Results}

	outputTestSuiteResult(suiteResult, config)
}

func outputTestSuiteResult(result TestSuiteResult, config testsuite.Config) {
	if consts.Flags.OutFormat == "json" {
		fmt.Println(outputJson(result, config))
	} else if consts.Flags.OutFormat == "csv" {
		fmt.Println(outputCsv(result, config))
	} else {
		fmt.Println("Wrong output type, specify json or csv")
	}
}

func outputJson(result TestSuiteResult, config testsuite.Config) string {
	out, _ := json.MarshalIndent(result, "", "    ")
	return string(out)
}

func outputCsv(result TestSuiteResult, config testsuite.Config) string {
	resMap := make(map[string]float64)
	for _, res := range result.Results {
		if len(res.Tests) > 0 {
			resMap[res.Name] = res.Score
		}
	}

	csvOutput := make([]string, 0)
	for _, t := range config.Tasks {
		res, ok := resMap[t.Name]
		if ok {
			csvOutput = append(csvOutput, fmt.Sprintf("%.2f", res))
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
