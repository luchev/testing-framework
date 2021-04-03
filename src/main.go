package main

import "C"

import (
	"flag"
	"log"

	"github.com/luchev/dtf/consts"
	"github.com/luchev/dtf/local"
	"github.com/luchev/dtf/master"
	"github.com/luchev/dtf/util"
)

func main() {
	consts.Init()

	if consts.Flags.Master {
		log.Println("Starting a Master service")
		util.InitWorkspace(consts.Settings.UploadsPath)
		master.SetupRoutes(consts.Flags.Port)
	} else if consts.Flags.Worker {
		// log.Println("Starting a Worker service")
		// util.InitWorkspace(consts.Settings.UploadsPath)
		// worker.SetupRoutes(consts.Flags.Port)
	} else if consts.Flags.SrcPath != "" && consts.Flags.Conf != "" {
		util.InitWorkspace(consts.Settings.UploadsPath)
		local.TestProject(consts.Flags.SrcPath, consts.Flags.Conf)
	} else {
		flag.Usage()
	}
}
