package response

import (
	"github.com/luchev/dtf/structs/task"
	"github.com/luchev/dtf/structs/test"
)

// Response represents the data provided to a go html template to render test results
type Response struct {
	PageTitle       string
	StudentIdentity map[string]string
	Tasks           []task.Result
	Errors          []test.Error
}
