package main

import (
	"fmt"
	"net/http"

	"github.com/TanishqM1/Orderbook/internal/handlers"
	"github.com/go-chi/chi"
	log "github.com/sirupsen/logrus"
)

// in this file, I setup the logger, mutex, as well as pass in the mutex to the handler.

func main() {
	log.SetReportCaller(true)

	var r *chi.Mux = chi.NewRouter()
	// pass to handler
	handlers.Handler(r)
	fmt.Println("Starting My Local Go API Service!")

	err := http.ListenAndServe("localhost:8000", r)

	// if for some reason the server does not start.
	if err != nil {
		log.Error(err)
	}
}
