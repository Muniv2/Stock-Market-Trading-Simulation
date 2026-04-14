#include "orderbook.h"
#include <ctime>
#include <chrono>
#include <atomic>
// ===== MUNIR : Buy - Side Implementation =====
void OrderBook :: placeBuyOrder (int price , int quantity , string
traderName ) {
OSTNode * node = new OSTNode ( price , ORDER_NODE ) ;
node->order.setPrice ( price ) ;
node->order.setQuantity ( quantity ) ;
node->order.setTraderName ( traderName ) ;
node->order.setTimestamp ( time ( nullptr ) ) ;
buyOST.insert ( node ) ;
cout << "\n[ ORDER ] " << traderName << " BUY " << quantity
<< " @ $" << price << "\n";
}
OSTNode * OrderBook :: getBestBid () {
if ( buyOST.totalNodes == 0) return nullptr ;
return buyOST . select ( buyOST . root , buyOST . totalNodes ) ;
}
OSTNode * OrderBook :: getBestAsk () {
if ( sellOST.totalNodes == 0) return nullptr ;
return sellOST.select ( sellOST . root , 1) ;
}
bool OrderBook :: matchOrders ( string & buyerName , string & sellerName, int & tradePrice , int & tradeQty ) {
OSTNode * bestBid = getBestBid () ;
OSTNode * bestAsk = getBestAsk () ;
if (! bestBid || ! bestAsk ) return false ;
if ( bestBid-> key >= bestAsk-> key ) {
int buyQty = bestBid-> order.getQuantity () ;
int sellQty = bestAsk-> order.getQuantity () ;
int tradeQuantity = min ( buyQty , sellQty ) ;
buyerName = bestBid-> order.getTraderName () ;
sellerName = bestAsk-> order.getTraderName () ;

tradePrice = bestAsk-> key ;
tradeQty = tradeQuantity ;
cout << "\n[ TRADE EXECUTED ] " << buyerName << " BUYS "
<< tradeQuantity << " shares from " << sellerName
<< " @ $" << tradePrice << "\n";

if ( buyQty == sellQty ) {
// Both orders fully filled
buyOST.deleteNode ( bestBid ) ;
sellOST.deleteNode ( bestAsk ) ;
} else if ( buyQty > sellQty ) {

bestBid-> order . setQuantity ( buyQty - tradeQuantity ) ;
sellOST.deleteNode ( bestAsk ) ;
} else {

bestAsk-> order . setQuantity ( sellQty - tradeQuantity ) ;
buyOST.deleteNode ( bestBid ) ;
}
return true ;
}
return false ;
}
