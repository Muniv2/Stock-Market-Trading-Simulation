# pragma once
# include "ost.h"
// Forward declaration for circular dependency
class Portfolio ;
class OrderBook {
    public :
        OST buyOST ;
        OST sellOST ;
        void placeBuyOrder (int price , int quantity , string traderName) ;
        OSTNode * getBestBid () ;
        OSTNode * getBestAsk () ;
        bool matchOrders ( string & buyerName , string & sellerName ,
        int& tradePrice , int& tradeQty ) ;
        void placeSellOrder (int price , int quantity , string
        traderName ) ;
        void cancelBuyOrder (int price , string traderName ) ;
        void cancelSellOrder (int price , string traderName ) ;
        int getOrderRangeCount (int x , int y ) ;
};