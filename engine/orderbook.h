// --- orderbook.h ---

#pragma once

#include <iostream>
#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <cstdint>
#include <memory>
#include <iterator>
#include <numeric>
#include <string>
#include <algorithm> // For std::min
#include <stdexcept>
#include <format> // For std::format in C++20

// Using namespace std is generally discouraged in header files, 
// but we'll respect its use from the original file while keeping it minimal.
using namespace std;

// --- ENUMS & TYPE ALIASES ---

enum class OrderType
{
    GoodTillCancel,
    FillAndKill
};

enum class Side
{
    Buy,
    Sell
};

using Price = std::int32_t;
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

// --- STRUCTS & DATA AGGREGATES ---

struct LevelInfo
{
    Price price_;
    Quantity quantity_;
};

using LevelInfos = std::vector<LevelInfo>;

struct TradeInfo
{
    OrderId orderid_;
    Price price_;
    Quantity quantity_;
};

using Trades = std::vector<class Trade>; // Forward declaration of Trade

// --- CLASS DEFINITIONS ---

// 1. OrderBookLevelInfo
class OrderBookLevelInfo
{
public:
    // Constructor defined inline
    OrderBookLevelInfo(const LevelInfos& asks, const LevelInfos& bids) :
        asks_(asks),
        bids_(bids) {}

    // Getters defined inline (implicitly inline)
    const LevelInfos& GetBids() const { return bids_; }
    const LevelInfos& GetAsks() const { return asks_; }

private:
    LevelInfos bids_;
    LevelInfos asks_;
};

// 2. Order
class Order
{
public:
    // Constructor defined inline
    Order(OrderType orderType, Side side, Price price, Quantity quantity, OrderId orderId) :
        orderType_(orderType),
        orderId_(orderId),
        price_(price),
        side_(side),
        initialQuantity_(quantity),
        remainingQuantity_(quantity) {}

    // Getters defined inline
    OrderId GetOrderId() const { return orderId_; }
    Side GetSide() const { return side_; }
    Price GetPrice() const { return price_; }
    OrderType GetOrderType() const { return orderType_; }
    Quantity GetInitialQuantity() const { return initialQuantity_; }
    Quantity GetRemainingQuantity() const { return remainingQuantity_; }
    Quantity FilledQuantity() const { return GetInitialQuantity() - GetRemainingQuantity(); }
    bool IsFilled() const { return GetRemainingQuantity() == 0; }

    // Fill method defined inline (modifies members, so it's NOT const)
    void Fill(Quantity quantity) {
        if (quantity > GetRemainingQuantity()) {
            // Using std::format is C++20 specific, ensure compiler support
            throw std::logic_error(std::format("Order ({}) cannot be filled for more than its remaining quantity", GetOrderId()));
        }
        remainingQuantity_ -= quantity;
    }

private:
    OrderType orderType_;
    OrderId orderId_;
    Price price_;
    Side side_;
    Quantity initialQuantity_;
    Quantity remainingQuantity_;
};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

// 3. OrderModify
class OrderModify
{
public:
    // Constructor defined inline
    OrderModify(OrderId orderId, Side side, Price price, Quantity quantity) :
        orderId_(orderId),
        side_(side),
        price_(price),
        quantity_(quantity) {}

    // Getters defined inline
    OrderId GetOrderId() const { return orderId_; }
    Price GetPrice() const { return price_; }
    Side GetSide() const { return side_; }
    Quantity GetQuantity() const { return quantity_; }

    // ToOrderPointer defined inline
    OrderPointer ToOrderPointer(OrderType type) const {
        return std::make_shared<Order>(type, GetSide(), GetPrice(), GetQuantity(), GetOrderId());
    }

private:
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity quantity_;
};

// 4. Trade
class Trade
{
public:
    // Constructor defined inline
    Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade) :
        bidTrade_{ bidTrade },
        askTrade_{ askTrade } {}

    // Getters defined inline (Note: Must be 'const' if they don't modify members)
    // IMPORTANT FIX: Original GetTrade() methods were not const, but should be.
    const TradeInfo& GetBidTrade() const { return bidTrade_; }
    const TradeInfo& GetAskTrade() const { return askTrade_; }

private:
    TradeInfo bidTrade_;
    TradeInfo askTrade_;
};


// 5. Orderbook
class Orderbook
{
private:
    // Internal struct for fast lookup and removal
    struct OrderEntry {
        OrderPointer order_{ nullptr };
        OrderPointers::iterator location_;
    };

    // Bids sorted descending (highest price first)
    std::map<Price, OrderPointers, std::greater<Price>> bids_; 
    // Asks sorted ascending (lowest price first)
    std::map<Price, OrderPointers, std::less<Price>> asks_;     
    // Lookup table for O(1) order access/cancellation
    std::unordered_map<OrderId, OrderEntry> orders_;

    // Private helper methods defined inline

    bool CanMatch(Side side, Price price) const {
        if (side == Side::Buy) {
            if (asks_.empty()) {
                return false;
            } else {
                const auto& [bestAsk, _] = *asks_.begin();
                return price >= bestAsk;
            }
        }
        // side == Side::Sell
        if (bids_.empty()) {
            return false;
        } else {
            const auto& [bestBid, _] = *bids_.begin();
            return price <= bestBid;
        }
    }
    
    Trades MatchOrders() {
        Trades trades;
        trades.reserve(orders_.size()); // Optimization

        while (true) {
            if (bids_.empty() || asks_.empty()) { break; }

            auto& [askPrice, asks] = *asks_.begin();
            auto& [bidPrice, bids] = *bids_.begin();

            if (bidPrice < askPrice) { break; }

            while (!bids.empty() && !asks.empty()) {
                auto& bid = bids.front();
                auto& ask = asks.front();

                Quantity quantity = std::min(bid->GetRemainingQuantity(), ask->GetRemainingQuantity());

                // Fill orders
                bid->Fill(quantity);
                ask->Fill(quantity);

                // Create Trade Info using the market-clearing price (bid or ask)
                // Using the ask price for the trade is a common convention
                trades.push_back(Trade{
                    TradeInfo{ bid->GetOrderId(), ask->GetPrice(), quantity },
                    TradeInfo{ ask->GetOrderId(), ask->GetPrice(), quantity }
                });

                // Clean up filled orders from OrderBook
                if (bid->IsFilled()) {
                    orders_.erase(bid->GetOrderId());
                    bids.pop_front();
                }
                if (ask->IsFilled()) {
                    orders_.erase(ask->GetOrderId());
                    asks.pop_front();
                }
            }

            // Clean up empty price levels
            if (bids.empty()) { bids_.erase(bidPrice); }
            if (asks.empty()) { asks_.erase(askPrice); }
        }

        // Handle FillAndKill (FAK) cleanup after matching logic
        if (!bids_.empty()){
            auto& [_, bids] = *bids_.begin();
            auto& order = bids.front();
            if (order->GetOrderType() == OrderType::FillAndKill && !order->IsFilled()){
                CancelOrder(order->GetOrderId());
            }
        }

        if (!asks_.empty()){
            auto& [_, asks] = *asks_.begin();
            auto& order = asks.front();
            if (order->GetOrderType() == OrderType::FillAndKill && !order->IsFilled()){
                CancelOrder(order->GetOrderId());
            }
        }

        return trades;
    }

    // CancelOrder implementation defined inline (since it's a private helper)
    void CancelOrder(OrderId orderId) {
        if (!orders_.contains(orderId)) {
            return;
        }

        const auto& [order, orderIterator] = orders_.at(orderId);
        orders_.erase(orderId);

        if (order->GetSide() == Side::Sell) {
            auto price = order->GetPrice();
            auto& orders = asks_.at(price);
            orders.erase(orderIterator);
            if (orders.empty()) {
                asks_.erase(price);
            }
        } else { // Side::Buy
            auto price = order->GetPrice();
            auto& orders = bids_.at(price);
            orders.erase(orderIterator);
            if (orders.empty()) {
                bids_.erase(price);
            }
        }
    }


public:
    // Public member methods defined inline

    Trades AddOrder(OrderPointer order) {
        if (orders_.contains(order->GetOrderId())) { return {}; }

        if (order->GetOrderType() == OrderType::FillAndKill && !CanMatch(order->GetSide(), order->GetPrice())) {
            return {};
        }

        OrderPointers::iterator iterator;
        if (order->GetSide() == Side::Buy) {
            auto& orders = bids_[order->GetPrice()];
            orders.push_back(order);
            // Use std::prev(orders.end()) as a safer way to get iterator to last element
            iterator = std::prev(orders.end()); 
        } else {
            auto& orders = asks_[order->GetPrice()];
            orders.push_back(order);
            iterator = std::prev(orders.end());
        }

        orders_.insert({ order->GetOrderId(), OrderEntry{ order, iterator } });
        return MatchOrders();
    }

    // Expose CancelOrder as a public method
    void CancelOrderPublic(OrderId orderId) {
        CancelOrder(orderId);
    }
    
    Trades MatchOrder(OrderModify order) {
        if (!orders_.contains(order.GetOrderId())) {
            return {};
        }

        const auto& [existingOrder, _] = orders_.at(order.GetOrderId());
        CancelOrder(order.GetOrderId());
        return AddOrder(order.ToOrderPointer(existingOrder->GetOrderType()));
    }

    std::size_t Size() const { return orders_.size(); }

    OrderBookLevelInfo GetOrderInfos() const {
        LevelInfos askinfos, bidinfos;
        bidinfos.reserve(orders_.size());
        askinfos.reserve(orders_.size());

        auto CreateLevelInfos = [](Price price, const OrderPointers& orders) {
            return LevelInfo{ price, std::accumulate(orders.begin(), orders.end(), (Quantity)0, [](Quantity runningSum, const OrderPointer& order) {
                return runningSum + order->GetRemainingQuantity();
            }) };
        };

        for (const auto& [price, orders] : bids_)
            bidinfos.push_back(CreateLevelInfos(price, orders));

        for (const auto& [price, orders] : asks_)
            askinfos.push_back(CreateLevelInfos(price, orders));
            
        return OrderBookLevelInfo(askinfos, bidinfos);
    }
};

// --- FREE FUNCTIONS ---

// Defined inline
inline OrderType setType(string type) {
    // Standardizing case for robustness
    std::transform(type.begin(), type.end(), type.begin(), ::tolower); 
    if (type == "goodtillcancel") {
        return OrderType::GoodTillCancel;
    } else {
        return OrderType::FillAndKill;
    }
}

// Defined inline
inline Side setSide(string side) {
    // Standardizing case for robustness
    std::transform(side.begin(), side.end(), side.begin(), ::tolower);
    if (side == "buy") {
        return Side::Buy;
    } else {
        return Side::Sell;
    }
}

// --- OPTIONAL UTILITY STRUCT ---

struct Counter{
    uint64_t count = 0;

    uint64_t GetNext(){
        return ++count;
    }
    uint64_t GetCurrent(){
        return count;
    }
};