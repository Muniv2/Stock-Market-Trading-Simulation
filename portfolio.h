# pragma once
# include "ost.h"
# include <map>
# include <string>

using namespace std;

class Portfolio {
private :
    long long tradeCounter ;
    // Map for O(log n) lookup by trader name
    map < string , OSTNode * > tradersByName ;

public :
    OST portfolioOST ;     // For ranking and percentile queries
    OST tradeHistoryOST ;  // For chronological trade history

    Portfolio () ;

    // Initializes a new trader and adds to the system
    void createProfile ( string name , string role , int amount ) ;

    // Handles wealth updates and maintains OST balance
    void updatePortfolio ( string name , int oldValue , int newValue ,
                         string role , int budget , int inventory ,
                         int pnl ) ;

    void recordTrade (int price , int quantity ,
                     string buyerName , string sellerName ) ;

    void settleTradeAmounts ( string buyerName , string sellerName ,
                            int tradePrice , int tradeQty ) ;

    int getTraderRank (int portfolioValue ) ;
    int getTraderPercentile (int portfolioValue ) ;
    void getRecentTrades (int n ) ;

    OSTNode * findTrader ( string name ) ;

    void clearNameMap () { tradersByName . clear () ; } // Public
//accessor for cleanup
};