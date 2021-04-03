package response

import (
	"github.com/luchev/dtf/structs/task"
	"github.com/luchev/dtf/structs/test"
)

// Response represents the data provided to a go html template to render test results
type Response struct {
	PageTitle string
	Tasks     []task.TaskResult
	Errors    []test.Error
}
