package testsuite

import (
	"path/filepath"

	"github.com/luchev/dtf/structs/task"
	"github.com/luchev/dtf/structs/test"
	"github.com/luchev/dtf/util"
)

type Result struct {
	StudentId map[string]string
	Results   []task.Result
}

func TestProject(projectPath string, suiteConfig Config) (*Result, *test.Error) {
	err := util.Unzip(projectPath, filepath.Dir(projectPath))
	if err != nil {
		return nil, &test.Error{Name: "Failed to extract project", Details: err.Error()}
	}

	_, _, err = util.ExecuteScript(suiteConfig.InitScript)
	if err != nil {
		return nil, &test.Error{Name: "Failed to init testing environoment"}
	}

	taskResults := make([]task.Result, 0)
	for _, suiteTask := range suiteConfig.Tasks {
		result := task.Result{Name: suiteTask.Name, PassingBuild: true, BuildMessage: "", Errors: nil, Tests: nil, Score: 0}
		_, stderr, err := util.ExecuteScript(suiteTask.InitScript)
		if err != nil {
			result.PassingBuild = false
			result.Errors = append(result.Errors, test.Error{Name: "Build failed", Details: stderr})
			taskResults = append(taskResults, result)
			continue
		}

		result.BuildMessage = stderr
		testResults := suiteTask.RunTestScript()
		result.Tests = append(result.Tests, testResults...)
		if suiteTask.MemoryPoints > 0 {
			result.Tests = append(result.Tests, suiteTask.MemoryLeakTest())
		}
		for _, t := range result.Tests {
			result.Score += t.Score
		}
		taskResults = append(taskResults, result)
	}

	return &Result{StudentId: nil, Results: taskResults}, nil
}
