package test

import (
	"bytes"
	"encoding/json"
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"mime/multipart"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"strconv"
	"strings"

	"github.com/joshdk/go-junit"
)

// TestResult represents a test with a name and an outcome
type TestResult struct {
	Name    string
	Passing bool
	Err     string
	Points  float64
}

func ParseJunitTests(suites []junit.Suite) (testResults []TestResult, allPoints float64) {
	for _, suite := range suites {
		for _, test := range suite.Tests {
			nameAndPoints := strings.Split(test.Name, ":")
			name := test.Name
			points := float64(1)
			if len(nameAndPoints) >= 2 {
				name = strings.Join(nameAndPoints[0:len(nameAndPoints)-1], ":")
				points, _ = strconv.ParseFloat(nameAndPoints[len(nameAndPoints)-1], 64)
			}

			result := TestResult{Name: name, Passing: true, Err: "", Points: points}
			if test.Error != nil {
				result.Passing = false
				result.Err = test.Error.Error()
				result.Points = 0
			} else {
				allPoints += points
			}

			testResults = append(testResults, result)
		}
	}

	return
}

func RunTests(names []string, srcDir string) (results []TestResult) {
	for _, test := range names {
		res, err := RunTest(srcDir, test)
		if err != nil {
			res = TestResult{
				Name:    test,
				Passing: false,
				Err:     fmt.Sprintf("Failed to run %s with err: %s", test, err.Error()),
				Points:  0,
			}
			log.Printf("Failed to run %s: %s", test, err.Error())
		}
		results = append(results, res)
	}
	return results
}

func RunTest(srcDir string, testName string) (result TestResult, err error) {
	runTests := exec.Command("bash", "-c", fmt.Sprintf("cd %s ; cargo junit --test-name %s", srcDir, testName))
	var stdout bytes.Buffer
	runTests.Stdout = &stdout
	err = runTests.Run()
	if err != nil {
		return
	}

	suites, err := junit.Ingest(stdout.Bytes())
	if err != nil {
		return TestResult{}, fmt.Errorf(stdout.String())
	}

	// Should have exactly 1 test
	for _, suite := range suites {
		for _, test := range suite.Tests {
			result = TestResult{Name: test.Name, Passing: true, Err: "", Points: 0}
			if test.Error != nil {
				result.Passing = false
				result.Err = test.Error.Error()
			}
		}
	}

	return
}

func RunTestsRemotely(names []string, codeArchivePath string, worker string) (results []TestResult, err error) {
	worker += "/test"
	srcArchive, err := os.Open(codeArchivePath)
	if err != nil {
		return nil, err
	}
	defer srcArchive.Close()

	body := &bytes.Buffer{}
	writer := multipart.NewWriter(body)

	testListWriter, err := writer.CreateFormField("testList")
	if err != nil {
		return nil, err
	}
	io.WriteString(testListWriter, strings.Join(names, ","))

	formFile, err := writer.CreateFormFile("codeZip", filepath.Base(srcArchive.Name()))
	if err != nil {
		return nil, err
	}

	io.Copy(formFile, srcArchive)
	writer.Close()
	request, err := http.NewRequest("POST", worker, body)
	if err != nil {
		return nil, err
	}

	request.Header.Add("Content-Type", writer.FormDataContentType())
	client := &http.Client{}
	response, err := client.Do(request)
	if err != nil {
		return nil, err
	}
	defer response.Body.Close()

	responseBody, err := ioutil.ReadAll(response.Body)
	if err != nil {
		return nil, err
	}
	responseString := string(responseBody)

	if response.StatusCode == http.StatusBadRequest {
		return nil, errors.New(responseString)
	}

	err = json.Unmarshal(responseBody, &results)
	if err != nil {
		return nil, err
	}

	return results, nil
}
