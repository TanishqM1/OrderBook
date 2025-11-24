// --- addOrder_ffi.h ---
#pragma once
#include <stdint.h>

// Linkage protection for C++ compilers
#ifdef __cplusplus
extern "C" {
#endif

typedef void* OrderBookAddress;

void AddOrderToEngine(
    OrderBookAddress book_ptr,
    int orderType,
    int side,
    int32_t price,
    uint32_t quantity,
    uint64_t orderId
);

OrderBookAddress CreateBook(const char* name);
void DestroyBook(OrderBookAddress book_ptr);

// End of linkage protection
#ifdef __cplusplus
}
#endif