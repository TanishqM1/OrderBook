# Orderbook Mock Trading Engine (In Progress)

C++ (engine), Go (backend APIs), Next.js (frontend) serving as a mock stock exchange.

#### This project is a mock electronic trading engine that simulates the core components of a real exchange orderbook. It includes:

- Limit order creation and management

- Bid/ask book structure

- Price–time priority queues

- Order matching logic (in progress)

- Trade generation (in progress)

- Clean, modern C++ architecture using smart pointers and strong typing

#### The engine models key market concepts such as:

- Limit orders (Buy/Sell)

- Order types (GTC, FAK)

- Price levels and aggregated quantities

- Trades and execution reports

- Data structures for bids and asks

trying to finish in ~ 2-3 weeks, with a full trading engine + minimal turn-based GUI
(ideally using testing to simulate bids and asks, and mimic how the engine reacts).

11/22 - OrderBook() implementation and explanation is finished. 

The schema is derived from CodingJesus's [Orderbook Video](https://www.youtube.com/watch?v=XeLWe0Cx_Lg&t=1258s)


# Roadmap

- implement tests for trading book using live input (thorough tests, must all pass)
- imeplement small GUI (can be cnosole based? to test with trading book)
- simulate a days of trades (stress test) for performance checks
- revisit code & improve performance
- add backend/frontend with APIs to enter deals into the OrderBook. (Go, Next.js, FFI bridge)

- Make entire program easily runnable, serving as a mock stock exchange with updates on the frontend.


11/23 - OrderBook() is finished, Go backend is initialized. Need to implement JSON parsing, error handling, and FFI bridge to C++ on the backned. 

# Upcoming architecture:

Our Go backend will listen for incoming api requests containing information for buy/sell orders. It will use C++ methods via a FFI bridge to invoke the methods using parsed JSON.

While the C++ code runs, and our engine is "on", it is essentially thread-blocked, meaning it is mimicking sleeping until it is invoked (a new order is pushed to a queue) in which it pops from the queue, adds it to the orderbook,
computes as needed, and goes back to sleep.

- implement FFI bridge

- send data in go over FFI bridge

- have some sort of listener in my CPP code (refactor it!) that waits for information to come in, and then act on it

- return information over the FFI bridge of ALL levels in our orderbook(s).

- make the frontend that sends json, and accepts the return json to show visuals.

when this is done, the project will be complete.