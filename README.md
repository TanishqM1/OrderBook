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

## 1.  C++ Engine Layer (The Core Logic) //**DONE**//

This layer is the heart of the system and contains the business-critical algorithms.

- Primary Service: Low-latency Order Matching and State Management.

- Function: Holds the active Orderbook state (all resting bids and asks in RAM) and executes the Price-Time Priority matching algorithm (AddOrder, MatchOrders).

- Output: Returns trade executions and market liquidity snapshots (GetOrderInfos).

## 2. C++ Server Layer (The Performance Gateway)

This layer wraps the Engine and exposes its functionality over a fast network protocol, acting as the service boundary.

- Primary Service: Network Communication and gRPC Endpoint.

- Function: Implements the gRPC service interfaces (defined in a .proto file) like PlaceOrder, CancelOrder, and GetMarketDepth. It translates incoming gRPC messages into native C++ object calls (Orderbook::AddOrder()) and serializes the C++ results back into gRPC responses.

- Why C++: Choosing C++ for the gRPC server maximizes speed by keeping the network protocol handling and the core matching logic within the same optimized, minimal-latency runtime.

## 3. Go Backend API (The External Interface) //**DONE**//

- Primary Service: Client Authentication and Routing/Traffic Management.

- Function: Handles user authentication (if required), provides external RESTful endpoints (HTTP/JSON) for the Frontend, and serves as the gRPC client. It translates incoming REST API calls into the required gRPC service calls to the C++ Server.

- Why Go: Go is used for its excellent concurrency model (goroutines) to handle high volumes of simultaneous network connections from external clients efficiently and for its robust library support for HTTP and gRPC client logic.

## 4. Frontend UI (Visualization and Input)

- Primary Service: Visualization and User Interaction.

- Function: Handles user input for placing orders, sends JSON requests to the Go Backend API, and receives streaming/polled market data. It renders the order book depth, trade history, and overall system state visually for the end-user.

- Technology: Next.js is chosen for its modern capabilities in building fast, complex, and data-driven user interfaces.
