package task

import (
	"bytes"
	"os/exec"
	"time"

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
	c1 := make(chan []test.TestResult, 1)
	go func() {
		cmd := exec.Command("bash", "-c", t.TestScript)
		var stdout bytes.Buffer
		cmd.Stdout = &stdout
		var stderr bytes.Buffer
		cmd.Stderr = &stderr

		cmd.Run()
		suites, err := junit.Ingest(stdout.Bytes())
		if err != nil {
			result := test.TestResult{
				Name: "Runtime error",
				Err: test.Error{
					Name:    "Runtime error",
					Details: stderr.String(),
				},
				Passing:  false,
				Score:    0,
				MaxScore: 0,
			}
			c1 <- []test.TestResult{result}
			return
		}

		testResults := test.ParseJunitTests(suites)
		if stderr.Len() > 0 {
			testResults = append(testResults, test.TestResult{
				Name: "Runtime error",
				Err: test.Error{
					Name:    "Runtime error",
					Details: stderr.String(),
				},
				Passing:  false,
				Score:    0,
				MaxScore: 0,
			})
		}

		c1 <- testResults
	}()

	select {
	case res := <-c1:
		return res
	case <-time.After(1 * time.Minute):
		return []test.TestResult{
			{
				Name:     "Execution timed out",
				Passing:  false,
				Err:      test.Error{Name: "Program execution timed out"},
				Score:    0,
				MaxScore: 0,
			},
		}
	}
}

func (t *Task) MemoryLeakTest() test.TestResult {
	c1 := make(chan test.TestResult, 1)
	go func() {
		cmd := exec.Command("bash", "-c", t.MemoryScript)
		var stdout bytes.Buffer
		cmd.Stdout = &stdout

		err := cmd.Run()
		if err == nil {
			c1 <- test.TestResult{
				Name:    "Memory leak test",
				Passing: true, Err: test.Error{},
				Score:    t.MemoryPoints,
				MaxScore: t.MemoryPoints,
			}
		} else {
			c1 <- test.TestResult{
				Name:     "Memory leak test",
				Passing:  false,
				Err:      test.Error{Name: "Memory leak detected", Details: "Memory leak detected"},
				Score:    0,
				MaxScore: t.MemoryPoints,
			}
		}
	}()

	select {
	case res := <-c1:
		return res
	case <-time.After(1 * time.Minute):
		return test.TestResult{
			Name:     "Memory leak test",
			Passing:  false,
			Err:      test.Error{Name: "Program execution timed out"},
			Score:    0,
			MaxScore: t.MemoryPoints,
		}
	}
}
