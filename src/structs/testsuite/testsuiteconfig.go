package testsuite

import "github.com/luchev/dtf/structs/task"

type SuiteConfig struct {
	InitScript            string      `yaml:"initScript"`
	Tasks                 []task.Task `yaml:"tasks"`
	StudentIdRegex        string      `yaml:"studentIdRegex"`
	StudentIdOutputFormat string      `yaml:"studentIdOutputFormat"`
}
