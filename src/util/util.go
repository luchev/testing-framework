package util

import (
	"archive/zip"
	"bytes"
	"errors"
	"fmt"
	"html/template"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"strings"

	"github.com/luchev/dtf/consts"
)

func RetrieveLocalFile(path string) (string, string, error) {
	input, err := ioutil.ReadFile(path)
	if err != nil {
		return "", "", err
	}

	tempDir, err := ioutil.TempDir(consts.Settings.UploadsPath, "")
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

func Unzip(src string, dest string) error {
	reader, err := zip.OpenReader(src)
	if err != nil {
		return fmt.Errorf("invalid zip archive: %s", filepath.Base(src))
	}
	defer reader.Close()

	for _, file := range reader.File {
		fpath := filepath.Join(dest, file.Name)

		if !strings.HasPrefix(fpath, filepath.Clean(dest)+string(os.PathSeparator)) {
			return fmt.Errorf("illegal file name %s", file.Name)
		}

		if file.FileInfo().IsDir() {
			os.MkdirAll(fpath, os.ModePerm)
			continue
		}

		if err = os.MkdirAll(filepath.Dir(fpath), os.ModePerm); err != nil {
			return fmt.Errorf("failed to create directory structure for %s", file.Name)
		}

		outFile, err := os.OpenFile(fpath, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, file.Mode())
		if err != nil {
			return fmt.Errorf("failed to create local file when extracting %s", file.Name)
		}

		inFile, err := file.Open()
		if err != nil {
			return fmt.Errorf("failed to open archived %s", file.Name)
		}

		_, err = io.Copy(outFile, inFile)

		outFile.Close()
		inFile.Close()

		if err != nil {
			return fmt.Errorf("failed to extract %s", file.Name)
		}
	}

	return nil
}

func ExecuteScript(script string) (string, string, error) {
	cmd := exec.Command("bash", "-c", script)
	var stdout bytes.Buffer
	cmd.Stdout = &stdout
	var stderr bytes.Buffer
	cmd.Stderr = &stderr

	err := cmd.Run()
	if err != nil {
		return stdout.String(), stderr.String(), err
	} else {
		return stdout.String(), stderr.String(), err
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

func EscapeNewLineHTML(input string) template.HTML {
	return template.HTML(strings.Replace(input, "\n", "<br />", -1))
}

func RetrieveFile(request *http.Request, fieldName string) (string, string, error) {
	file, handle, err := request.FormFile(fieldName)
	if err != nil {
		return "", "", errors.New("no file provided")
	}
	fileName := handle.Filename

	defer file.Close()
	tempDir, err := ioutil.TempDir(consts.Settings.UploadsPath, "")
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
