package util

import (
	"archive/zip"
	"bytes"
	"errors"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	"gopkg.in/yaml.v2"
)

func RetrieveLocalFile(path string) (string, string, error) {
	input, err := ioutil.ReadFile(path)
	if err != nil {
		return "", "", err
	}

	tempDir, err := ioutil.TempDir("uploads", "")
	if err != nil {
		return "", "", err
	}

	fileName := filepath.Base(path)
	err = ioutil.WriteFile(tempDir+"/"+fileName, input, os.ModePerm)
	if err != nil {
		fmt.Println(err)
	}

	return tempDir, fileName, nil
}

func Unzip(src string, dest string) ([]string, error) {
	reader, err := zip.OpenReader(src)
	if err != nil {
		fmt.Println(err)
		return nil, err
	}
	defer reader.Close()

	sourceFiles := make([]string, 0)
	for _, file := range reader.File {
		fpath := filepath.Join(dest, file.Name)

		if !strings.HasPrefix(fpath, filepath.Clean(dest)+string(os.PathSeparator)) {
			return sourceFiles, fmt.Errorf("%s: illegal file path", fpath)
		}
		if file.FileInfo().IsDir() {
			os.MkdirAll(fpath, os.ModePerm)
			continue
		}

		if err = os.MkdirAll(filepath.Dir(fpath), os.ModePerm); err != nil {
			return sourceFiles, err
		}

		outFile, err := os.OpenFile(fpath, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, file.Mode())
		if err != nil {
			return sourceFiles, err
		}

		inFile, err := file.Open()
		if err != nil {
			return sourceFiles, err
		}

		if filepath.Ext(file.Name) == ".rs" {
			sourceFiles = append(sourceFiles, file.Name)
		}

		_, err = io.Copy(outFile, inFile)

		outFile.Close()
		inFile.Close()

		if err != nil {
			return sourceFiles, err
		}
	}

	return sourceFiles, nil
}

func ExecuteScriptFile(path string) (string, error) {
	base := filepath.Base(path)
	dir := filepath.Dir(path)
	cmd := exec.Command("bash", "-c", fmt.Sprintf("cd %s; ./%s", dir, base))
	var stdout bytes.Buffer
	cmd.Stdout = &stdout
	var stderr bytes.Buffer
	cmd.Stderr = &stderr

	err := cmd.Run()
	if err != nil {
		return stderr.String(), err
	} else {
		return stdout.String(), nil
	}
}

func ExecuteScript(script string) (string, error) {
	cmd := exec.Command("bash", "-c", script)
	var stdout bytes.Buffer
	cmd.Stdout = &stdout
	var stderr bytes.Buffer
	cmd.Stderr = &stderr

	err := cmd.Run()
	if err != nil {
		return stderr.String(), err
	} else {
		return stdout.String(), nil
	}
}

func InitWorkspace(uploadDir string) {
	if _, err := os.Stat(uploadDir); os.IsNotExist(err) {
		err := os.Mkdir(uploadDir, os.ModePerm)
		if err != nil {
			log.Fatal(err)
		}
		return
	}

	dir, err := os.Open(uploadDir)
	if err != nil {
		log.Fatal(err)
	}
	defer dir.Close()

	files, err := dir.Readdirnames(-1)
	if err != nil {
		log.Fatal(err)
	}

	for _, fileName := range files {
		err = os.RemoveAll(filepath.Join(uploadDir, fileName))
		if err != nil {
			log.Fatal(err)
		}
	}
}

func UnmarshalYamlFile(path string, out interface{}, varMap map[string]string) error {
	file, err := os.Open(path)
	if err != nil {
		return err
	}
	readBytes, err := io.ReadAll(file)
	for variable, value := range varMap {
		readBytes = bytes.ReplaceAll(readBytes, []byte(variable), []byte(value))
	}
	if err != nil {
		return err
	}

	err = yaml.Unmarshal(readBytes, out)
	if err != nil {
		return err
	}

	return nil
}

func EscapeNewLineHTML(input string) string {
	return strings.Replace(input, "\n", "<br>", -1)
}

func RetrieveFile(request *http.Request, fieldName string) (string, string, error) {
	file, handle, err := request.FormFile(fieldName)
	if err != nil {
		return "", "", errors.New("No file provided")
	}
	fileName := handle.Filename

	defer file.Close()
	tempDir, err := ioutil.TempDir("uploads", "")
	if err != nil {
		return "", "", err
	}

	archiveFile, err := os.OpenFile(tempDir+"/"+fileName, os.O_CREATE|os.O_WRONLY, os.ModePerm)
	if err != nil {
		fmt.Println(err)
	}

	uploadedBytes, err := ioutil.ReadAll(file)
	if err != nil {
		return "", "", nil
	}

	archiveFile.Write(uploadedBytes)

	return tempDir, fileName, nil
}
