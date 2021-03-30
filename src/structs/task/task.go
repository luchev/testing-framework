package task

import (
	"bytes"
	"os/exec"

	"github.com/joshdk/go-junit"
	"github.com/luchev/dtf/structs/error"
	"github.com/luchev/dtf/structs/test"
)

type Task struct {
	Name         string  `yaml:"name"`
	TestScript   string  `yaml:"testScript"`
	MemoryScript string  `yaml:"memoryScript"`
	MemoryPoints float64 `yaml:"memoryPoints"`
}

func (t *Task) RunTestScript() (result TaskResult) {
	cmd := exec.Command("bash", "-c", t.TestScript)
	var stdout bytes.Buffer
	cmd.Stdout = &stdout
	var stderr bytes.Buffer
	cmd.Stderr = &stderr

	err := cmd.Run()
	if stdout.Len() == 0 {
		result.PassingBuild = false
		result.BuildMessage = stderr.String()
		result.Errors = append(result.Errors, error.Error{Name: "Build error", Details: "Build failed"})
		return
	}

	suites, err := junit.Ingest(stdout.Bytes())
	if err != nil {
		result.PassingBuild = false
		result.Errors = append(result.Errors, error.Error{Name: "Build error", Details: err.Error()})
	} else {
		result.PassingBuild = true
		result.Tests, result.Points = test.ParseJunitTests(suites)
	}
	return
}

func (t *Task) HasMemoryLeak() bool {
	cmd := exec.Command("bash", "-c", t.MemoryScript)
	var stdout bytes.Buffer
	cmd.Stdout = &stdout

	err := cmd.Run()
	if err == nil {
		return false
	} else {
		return true
	}
}
