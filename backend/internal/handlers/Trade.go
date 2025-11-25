package handlers

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"net/url"
	"strconv"

	"github.com/TanishqM1/Orderbook/api"
	log "github.com/sirupsen/logrus"
)

// The logic for GetNextOrderID is assumed to be correctly defined and thread-safe within the api package.

func Trade(w http.ResponseWriter, r *http.Request) {
	// 1. Decode the incoming JSON request body into the api.AddFields struct
	var params = api.AddFields{}
	err := json.NewDecoder(r.Body).Decode(&params)

	fmt.Println(params)

	if err != nil {
		log.Error(err)
		api.HandleRequestError(w, err)
		return
	}

	// 2. Generate unique OrderID on the Go side using the thread-safe utility
	orderId := api.GetNextOrderId()
	fmt.Print(orderId)

	// 3. Format parameters for the C++ HTTP server (using query strings)
	urlValues := url.Values{}
	urlValues.Set("orderid", strconv.FormatUint(orderId, 10))
	urlValues.Set("tradetype", params.TradeType)
	urlValues.Set("side", params.Side)
	urlValues.Set("price", strconv.Itoa(params.Price))
	urlValues.Set("quantity", strconv.Itoa(params.Quantity))
	urlValues.Set("book", params.Name)

	client := http.Client{}

	// C++ server runs on :6060 with the /trade endpoint
	cppServerURL := fmt.Sprintf("http://localhost:6060/trade?%s", urlValues.Encode())

	log.Debugf("Forwarding trade request to C++ engine: %s", cppServerURL)

	// Create and send a new POST request to the C++ server (query params carry data)
	cppReq, err := http.NewRequest("POST", cppServerURL, nil)
	if err != nil {
		log.Errorf("Failed to create C++ request: %v", err)
		api.HandleInternalError(w)
		return
	}

	// 4. Reroute request to local C++ server on :6060
	cppResp, err := client.Do(cppReq)
	if err != nil {
		log.Errorf("Failed to connect to C++ engine at :6060. Is the C++ server running? Error: %v", err)
		api.HandleInternalError(w)
		return
	}
	defer cppResp.Body.Close()

	// 5. Proxy the response (status code and body) back to the original client
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(cppResp.StatusCode)

	if _, err := io.Copy(w, cppResp.Body); err != nil {
		log.Errorf("Failed to proxy response body: %v", err)
	}

	fmt.Printf("\nProcessed new order with ID: %d", orderId)
}
