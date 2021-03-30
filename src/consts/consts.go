package consts

import (
	"flag"
	"log"

	"github.com/luchev/dtf/util"
)

// Flags specifies the different args
type flags struct {
	SrcPath   string
	Conf      string
	OutFormat string
}

type settings struct {
	UploadsPath string   `yaml:"uploadDirPath"`
	ConfigsPath string   `yaml:"configDirPath"`
	Vars        []string `yaml:"vars"`
}

const baseConfigPath string = "config/base.yaml"

var Settings settings
var Flags flags

func Init() {
	initFlags()
	initSettings()
}

func initFlags() {
	flag.StringVar(&Flags.SrcPath, "file", "", "Test only one file")
	flag.StringVar(&Flags.Conf, "conf", "", "Path to config file")
	flag.StringVar(&Flags.OutFormat, "out", "json", "Detailed json or short csv")
	flag.Parse()
}

func initSettings() {
	err := util.UnmarshalYamlFile(baseConfigPath, &Settings, nil)
	if err != nil {
		log.Fatal(err)
	}
}
