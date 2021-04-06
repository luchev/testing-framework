package testsuite

import (
	"path/filepath"

	"github.com/luchev/dtf/structs/task"
	"github.com/luchev/dtf/yaml"
)

type Config struct {
	InitScript            string      `yaml:"initScript"`
	Tasks                 []task.Task `yaml:"tasks"`
	StudentIdRegex        string      `yaml:"studentIdRegex"`
	StudentIdOutputFormat string      `yaml:"studentIdOutputFormat"`
}

func UnmarshalConfig(configName string, projectPath string) (Config, error) {
	varMap := make(map[string]string)
	workDir := filepath.Dir(projectPath)
	varMap["${PROJECT_DIR}"], _ = filepath.Abs(workDir)
	varMap["${CONFIG_DIR}"], _ = filepath.Abs(filepath.Dir(configName))
	var suiteConfig Config
	err := yaml.UnmarshalYamlFile(configName, &suiteConfig, varMap)
	return suiteConfig, err
}
