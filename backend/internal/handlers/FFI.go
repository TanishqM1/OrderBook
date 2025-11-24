package handlers

import (
	"encoding/json"
	"net/http"
	"sync"

	"github.com/TanishqM1/Orderbook/api"
	log "github.com/sirupsen/logrus"
)

var wg = sync.WaitGroup{}

func Trade(w http.ResponseWriter, r *http.Request) {
	var params = api.Fields{}

	err := json.NewDecoder(r.Body).Decode(&params)

	if err != nil {
		log.Error(err)
		api.HandleRequestError(w, err)
		return
	}

	// now params has all the values from our JSON.

}
