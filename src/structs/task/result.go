package task

import (
	"github.com/luchev/dtf/structs/test"
)

// Result represents a whole project with all tests
type Result struct {
	Name         string
	PassingBuild bool
	BuildMessage string
	Errors       []test.Error
	Tests        []test.TestResult
	Score        float64
}
