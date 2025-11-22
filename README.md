# Orderbook Mock Trading Engine (In Progress)

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

#### Currently Implemented:
- Strongly typed enums (OrderType, Side)

- Order object with full fill logic and safety checks

- TradeInfo + Trade aggregation model

- LevelInfo and OrderBookLevelInfo for orderbook snapshots

- Shared pointer–based order storage (OrderPointer, OrderPointers)

- Order modification object (OrderModify)


trying to finish in ~ 2-3 weeks, with a full trading engine + minimal turn-based GUI
(ideally using testing to simulate bids and asks, and mimic how the engine reacts).