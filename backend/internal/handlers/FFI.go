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
	// automatically parses the json. the json is in this schema:
	// Type     string `json:"type"`
	// Side     string `json::"side"`
	// Price    string `json::"price"`
	// Quantity string `json::"quantity"`

	if err != nil {
		log.Error(err)
		api.HandleRequestError(w, err)
		return
	}

	// now params has all the values from our JSON. We need to send this over to our C++ engine via FFI.
	// we just need to invoke a method in the C++ code to add the order to a queue (which the CPP code will read from).
	//  we will add an object containing type (int), side (int), price (int), quantity (int)

	// then, the CPP code will convert it into an "Order" object, and add it to the orderbook.

}
