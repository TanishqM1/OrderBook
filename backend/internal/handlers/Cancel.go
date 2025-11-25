package handlers

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"net/url"
	"strconv"
	"strings"

	"github.com/TanishqM1/Orderbook/api"
	log "github.com/sirupsen/logrus"
)

func Cancel(w http.ResponseWriter, r *http.Request) {
	var params = api.CancelFields{}
	err := json.NewDecoder(r.Body).Decode(&params)

	if err != nil {
		log.Error(err)
		api.HandleRequestError(w, err)
		return
	}

	fmt.Printf("\nMyParams: %v", params)

	if params.OrderId == 0 {
		api.HandleRequestError(w, fmt.Errorf("orderId field is required, and cannot be zero"))
	}

	URL_Values := url.Values{}
	URL_Values.Set("orderid", strconv.FormatUint(uint64(params.OrderId), 10))
	URL_Values.Set("book", params.Book)

	reqBody := strings.NewReader(URL_Values.Encode())

	client := http.Client{}

	cppServerURL := "http://localhost:6060/cancel"

	log.Debugf("Forwarding cancel request to C++ engine: %s with body: %s", cppServerURL, URL_Values.Encode())

	cppReq, err := http.NewRequest("POST", cppServerURL, reqBody)
	if err != nil {
		log.Errorf("Failed to create C++ request: %v", err)
		api.HandleInternalError(w)
		return
	}

	cppReq.Header.Set("Content-Type", "application/x-www-form-urlencoded")
	cppResp, err := client.Do(cppReq)

	if err != nil {
		log.Errorf("Failed to connect to C++ engine at :6060. Is the C++ server running? Error: %v", err)
		api.HandleInternalError(w)
		return
	}
	defer cppResp.Body.Close()

	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(cppResp.StatusCode)

	if _, err := io.Copy(w, cppResp.Body); err != nil {
		log.Errorf("Failed to cop proxy response body: %v", err)
	}

	fmt.Printf("\nAttempted to Cancel Order: %d", params.OrderId)
}
