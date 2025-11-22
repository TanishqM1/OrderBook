#include <iostream>
#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <cstdint>
#include <vector>
#include <format>
#include <stdexcept>
#include <memory>


enum class OrderType{
    GoodTillCancel,
    FillAndKill
};

// "Order"s will have a Side. Side::Buy or Side::Sell
enum class Side{
    Buy,
    Sell
};

// alias types
using Price = std::int32_t; // price can be negative
using Quantity = std::uint32_t;
using OrderId = std::uint64_t;

// in cpp we denote "member varaibles" (i.e not parameters) with a "_".

struct LevelInfo{
    Price price_;
    Quantity quantity_;
};

// LevelInfos stores all the Quantity's at a certain price (level).
using LevelInfos = std::vector<LevelInfo>;

// OrderBookLevelInfo stores the vectors for asks and bids for all prices.
class OrderBookLevelInfo{
    // we need seperate vectors for bids and asks
    public:
        OrderBookLevelInfo(const LevelInfos& asks, const LevelInfos& bids):
        // constructor instantiation
        asks_(asks),
        bids_(bids) {}

        const LevelInfos& GetBids() const { return bids_; }
        const LevelInfos& GetAsks() const { return asks_;}
    
    private:
        LevelInfos bids_;
        LevelInfos asks_;
};

// What is added to the order book? Objets that have the order type, key, side, price, quantity, and bool(s) for filled or not
// Order stores instances of an order (with all needed properties).
class Order {
    // A PUBLIC constructor can initialize private fields.
    public:
        Order(OrderType orderType, Side side, Price price, Quantity quantity, OrderId orderId): 
            orderType_(orderType),
            orderId_(orderId),
            price_(price),
            side_(side),
            initialQuantity_(quantity),
            remainingQuantity_(quantity) {}

        OrderId GetOrderId() const { return orderId_; }
        Side GetSide() const { return side_; }
        Price GetPrice() const { return price_; }
        OrderType GetOrderType() const { return orderType_; }
        Quantity GetInitialQuantity() const { return initialQuantity_; }
        Quantity GetRemainingQuantity() const { return remainingQuantity_; }
        Quantity FilledQuantity() const { return GetInitialQuantity() - GetRemainingQuantity();}

        void Fill(Quantity quantity){
            // validate if the # of orders can actually be filled
            if (quantity > GetRemainingQuantity()){
                throw std::logic_error(std::format("Order ({}) cannot be filled for more than it's remaining quantity", GetOrderId()));
            }

            remainingQuantity_ -= quantity; // it has been filled
        }

        // the reason we need this private section here is because without it, we declare the variables in our public: modifier, but never assign them a type.
    private:
        OrderType orderType_;
        OrderId orderId_;
        Price price_;
        Side side_;
        Quantity initialQuantity_;
        Quantity remainingQuantity_;
};

// Orders go into multiple data structures, so we will keep a pointer to orders. (reference semantics) so we can easily reference them.
// std::make_shared<type>(); allocates order(s) on the heap, and returns a pointer pointing at it.

// So here, we have a vector of POINTERS to orders.
using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

// Common functionality we need to support for orders:

// Add() => we need a new order.
// Cancel() => we need an existing valid order id.
// Modify() => we need a way to modify existing orders, and we need to retrieve orders in a well manner.

class OrderModify{
    public:
        OrderModify(OrderId orderId, Side side, Price price, Quantity quantity):
        orderId_(orderId),
        side_(side),
        price_(price),
        quantity_(quantity) {}

    OrderId GetOrderId() const {return orderId_;}
    Price GetPrice() const {return price_;}
    Side GetSide() const {return side_;}
    Quantity GetQuantity() const {return quantity_;}

    // "const" in this function denotes the function does NOT modify any member variables.
    OrderPointer ToOrderPointer(OrderType type) const {
        return std::make_shared<Order>(type, GetOrderId(), GetSide(), GetPrice(), GetQuantity());
    }

    private:
        OrderId orderId_;
        Side side_;
        Price price_;
        Quantity quantity_;
};

// TradeInfo may exist by itself, but Trade can contain or reference one or more TradeInfo objects.
struct TradeInfo{
    OrderId orderid_;
    Price price_;
    Quantity quantity_;
};

// A trade consists of a bid and ask, which will hava TradeInfo objects for eahch.
class Trade{
    Trade(const TradeInfo& bidTrade, const TradeInfo& askTrade):
    bidTrade_ { bidTrade},
    askTrade_ { askTrade} {}

    const TradeInfo& GetBidTrade(){return bidTrade_;}
    const TradeInfo& GetAskTrade(){return askTrade_;}

    private:
        TradeInfo bidTrade_;
        TradeInfo askTrade_;
};


// vector of trade object.
using Trades = std::vector<Trade>;

// need to represent bids and asks. 
// Bids are sorted in descending order from the best bid (highest buyprice) and Asks are sorted in ascending order from the lowest ask (lowest askrpice). 

class Orderbook{
    private:
// VIDEO 1: 21:30 MINUTEs
};

int main(){

    return 0;
}