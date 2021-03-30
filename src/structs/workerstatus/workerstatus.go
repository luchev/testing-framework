package workerstatus

import (
	"log"
	"net/http"
)

// WorkerStatus represents the data provided to a go html template to render worker statuses
type WorkerStatus struct {
	URL    string
	Err    string
	Active bool
}

func PingWorkers(workers map[string]struct{}) []WorkerStatus {
	statuses := make([]WorkerStatus, 0)
	log.Printf("Pinging workers")
	for worker := range workers {
		statuses = append(statuses, WorkerStatus{URL: worker, Err: "", Active: true})
		_, err := http.Get(worker + "/ping")
		if err != nil {
			statuses[len(statuses)-1].Err = err.Error()
			statuses[len(statuses)-1].Active = false
		}
	}

	return statuses
}

func GetActiveWorkers(workers map[string]struct{}) []string {
	activeWorkers := make([]string, 0)
	for _, worker := range PingWorkers(workers) {
		if worker.Active {
			activeWorkers = append(activeWorkers, worker.URL)
		}
	}
	return activeWorkers
}
