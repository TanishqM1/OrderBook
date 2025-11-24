#include "orderbook.h"
#include <iostream>
#include <string>
#include <cstring>
#include "addOrder_ffi.h"
// MUST CREATE orderbook.h TOO USE!


    void AddOrderToEngine(
        OrderBookAddress book_ptr,
        int orderType,
        int side,
        int32_t price,
        uint32_t quantity,
        uint64_t orderId
    )
    {
        // convert opaque pointer (because it cannot compile without being opaque in our .h  file) back to an 
        // "Orderbook" pointer.
        Orderbook* book = static_cast<Orderbook*>(book_ptr);
        // Convert C variables to C++ types
        OrderType type = static_cast<OrderType>(orderType);
        Side s = static_cast<Side>(side);
        try{
            // create a new OrderPointer in CPP
            OrderPointer new_order = make_shared<Order>(type, s, price, quantity, orderId);
            // Call target function
            book->AddOrder(new_order);
        }catch(const std::exception& error){
            std::cerr << "\nAddOrder failed" << error.what();
        }
    }

    OrderBookAddress CreateBook(const char* name){
        // LIVES ON HEAP.
        Orderbook* new_book = new Orderbook();
        if (name){
            std::cout << "\n Created new orderbook for sumbol: " << name;
        }
        return static_cast<OrderBookAddress>(new_book);
    }

    void DestroyBook(OrderBookAddress book_ptr){
        delete static_cast<Orderbook*>(book_ptr);
    }