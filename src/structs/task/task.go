package task

import (
	"bytes"
	"os/exec"

	"github.com/joshdk/go-junit"
	"github.com/luchev/dtf/structs/test"
)

type Task struct {
	Name         string  `yaml:"name"`
	InitScript   string  `yaml:"initScript"`
	TestScript   string  `yaml:"testScript"`
	MemoryScript string  `yaml:"memoryScript"`
	MemoryPoints float64 `yaml:"memoryPoints"`
}

func (t *Task) RunTestScript() []test.TestResult {
	cmd := exec.Command("bash", "-c", t.TestScript)
	var stdout bytes.Buffer
	cmd.Stdout = &stdout
	var stderr bytes.Buffer
	cmd.Stderr = &stderr

	cmd.Run()

	suites, _ := junit.Ingest(stdout.Bytes())
	return test.ParseJunitTests(suites)
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

func (t *Task) MemoryLeakTest() test.TestResult {
	cmd := exec.Command("bash", "-c", t.MemoryScript)
	var stdout bytes.Buffer
	cmd.Stdout = &stdout

	err := cmd.Run()
	if err == nil {
		return test.TestResult{"Memory leak test", true, "", t.MemoryPoints}
	} else {
		return test.TestResult{"Memory leak test", false, "Memory leak detected", 0}
	}
}
