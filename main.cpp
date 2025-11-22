#include <iostream>
#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <cstdint>
#include <vector>


enum class OrderType{
    GoodTillCancel,
    FillAndKill
};

// "Order"s will have a type: Side::Buy or Side::Sell
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

using LevelInfos = std::vector<LevelInfo>;

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
    
        // the reason we need this private section here is because without it, we declare the variables in our public: modifier, but never assign them a type.
    private:
        OrderType orderType_;
        OrderId orderId_;
        Price price_;
        Side side_;
        Quantity initialQuantity_;
        Quantity remainingQuantity_;
};




int main(){

    return 0;
}