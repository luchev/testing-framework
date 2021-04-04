package yaml

import (
	"bytes"
	"io"
	"os"

	"gopkg.in/yaml.v2"
)

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
