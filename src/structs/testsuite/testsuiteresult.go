package testsuite

import "github.com/luchev/dtf/structs/task"

type TestSuiteResult struct {
	StudentId map[string]string
	Results   []task.TaskResult
}
