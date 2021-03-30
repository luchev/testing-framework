package main

import (
	"flag"

	"github.com/luchev/dtf/consts"
	"github.com/luchev/dtf/local"
	"github.com/luchev/dtf/util"
)

func main() {
	consts.Init()

	if consts.Flags.SrcPath != "" && consts.Flags.Conf != "" {
		util.InitWorkspace(consts.Settings.UploadsPath)
		local.TestProject(consts.Flags.SrcPath, consts.Flags.Conf)
	} else {
		flag.Usage()
	}
}
