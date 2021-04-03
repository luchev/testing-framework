package task

import (
	"github.com/luchev/dtf/structs/test"
)

// TaskResult represents a whole project with all tests
type TaskResult struct {
	Name         string
	PassingBuild bool
	BuildMessage string
	Errors       []test.Error
	Tests        []test.TestResult
	Points       float64
}
