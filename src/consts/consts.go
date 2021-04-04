package consts

import (
	"flag"
	"log"
	"os"
	"path/filepath"

	"github.com/luchev/dtf/yaml"
)

// Flags specifies the different args
type flags struct {
	Master    bool
	Worker    bool
	Port      int
	SrcPath   string
	Conf      string
	OutFormat string
}

type settings struct {
	UploadsPath string   `yaml:"uploadDirPath"`
	ConfigsPath string   `yaml:"configDirPath"`
	Vars        []string `yaml:"vars"`
}

const ConfigDir = "config/"
const baseConfigPath string = ConfigDir + "base.yaml"

var Settings settings
var Flags flags
var Configs []string

func Init() {
	initFlags()
	initSettings()
	initConfigs()
}

func initFlags() {
	flag.BoolVar(&Flags.Master, "master", false, "Start a master service")
	flag.BoolVar(&Flags.Worker, "worker", false, "Start a worker service")
	flag.IntVar(&Flags.Port, "port", 80, "Port to start the service on")
	flag.StringVar(&Flags.SrcPath, "file", "", "Test only one file")
	flag.StringVar(&Flags.Conf, "conf", "", "Path to config file")
	flag.StringVar(&Flags.OutFormat, "out", "json", "Detailed json or short csv")
	flag.Parse()
}

func initSettings() {
	err := yaml.UnmarshalYamlFile(baseConfigPath, &Settings, nil)
	if err != nil {
		log.Fatal(err)
	}
}

func initConfigs() {
	err := filepath.Walk(ConfigDir, func(path string, info os.FileInfo, err error) error {
		if filepath.Ext(path) == ".yaml" && path != baseConfigPath {
			Configs = append(Configs, path)
		}
		return nil
	})
	if err != nil {
		log.Fatal("Failed to retrieve configs", err)
	}
}
