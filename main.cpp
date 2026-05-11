# include "orderbook.h"
# include "portfolio.h"
OrderBook orderBook ;
Portfolio portfolio ;

// ===== ISMAIL : Menu & Input Handlers =====
void displayMenu () {
cout << "\n=== Trading System ===\n";
cout << "0. Create Profile | 1. Buy | 2. Sell \n";
cout << "3. Market | 4. Cancel Buy | 5. Cancel Sell \n";
cout << "6. Trader Stats\n";
cout << "9. Exit \n";
}
void handleCreateProfile () {
string name , role ;
int amount ;
cout << " Name : "; cin >> name ;
cout << " Role ( buyer / seller ): "; cin >> role ;
cout << " Amount : "; cin >> amount ;
portfolio . createProfile ( name , role , amount ) ;
}


void handleBuyOrder () {
int price , qty ;
string name ;
cout << " Trader : "; cin >> name ;
cout << " Price : "; cin >> price ;
cout << " Qty : "; cin >> qty ;
orderBook . placeBuyOrder ( price , qty , name ) ;
// CRITICAL ATOMICITY SEQUENCE : Do NOT add code between matchOrders and settleTradeAmounts
// If matchOrders returns true but program crashes before settleTradeAmounts completes ,
// Order Books and Portfolios will be out of sync , violating data consistency .
// In production , wrap this sequence in a database transaction .
// For this project , keep these calls back -to - back with no interleaving .
string buyer , seller ;
int tradePrice , tradeQty ;
if ( orderBook . matchOrders ( buyer , seller , tradePrice , tradeQty
) ) {
portfolio . recordTrade ( tradePrice , tradeQty , buyer , seller
) ;
portfolio . settleTradeAmounts ( buyer , seller , tradePrice ,
tradeQty ) ; // MUST complete before UI loop continues
}
}
void handleSellOrder () {
int price , qty ;
string name ;
cout << " Trader : "; cin >> name ;
cout << " Price : "; cin >> price ;
cout << " Qty : "; cin >> qty ;
orderBook . placeSellOrder ( price , qty , name ) ;
// CRITICAL ATOMICITY SEQUENCE : Do NOT add code between matchOrders and settleTradeAmounts
// If matchOrders returns true but program crashes before settleTradeAmounts completes ,
// Order Books and Portfolios will be out of sync , violating data consistency .
// In production , wrap this sequence in a database transaction .
// For this project , keep these calls back -to - back with no interleaving .
string buyer , seller ;
int tradePrice , tradeQty ;
if ( orderBook . matchOrders ( buyer , seller , tradePrice , tradeQty
) ) {
    portfolio . recordTrade ( tradePrice , tradeQty , buyer , seller
) ;
portfolio . settleTradeAmounts ( buyer , seller , tradePrice ,
tradeQty ) ; // MUST complete before UI loop continues
}
}
// ===== AHMAD : Query & Stats Handlers =====
void displayMarket () {
OSTNode * bid = orderBook . getBestBid () ;
OSTNode * ask = orderBook . getBestAsk () ;
cout << "\n[ MARKET ]\n";
if ( bid ) cout << " Best Bid : $" << bid -> key << "\n";
if ( ask ) cout << " Best Ask : $" << ask -> key << "\n";
}
void handleCancelBuy () {
int price ;
string name ;
cout << " Trader : "; cin >> name ;
cout << " Price : "; cin >> price ;
orderBook . cancelBuyOrder ( price , name ) ;
}
void handleCancelSell () {
int price ;
string name ;
cout << " Trader : "; cin >> name ;
cout << " Price : "; cin >> price ;
orderBook . cancelSellOrder ( price , name ) ;
}

void handleTraderStats() {
    string name;
    cout << "Trader name: "; cin >> name;
    OSTNode* node = portfolio.findTrader(name);
    if (!node) { cout << "[ERROR] Trader not found.\n"; return; }
    int r = portfolio.getTraderRank(node->key);
    int p = portfolio.getTraderPercentile(node->key);
    cout << "\n[TRADER STATS] " << name << "\n";
    cout << "------------------------------------------\n";
    cout << "Role          : " << node->trader.getRole() << "\n";
    cout << "Portfolio Val : $" << node->trader.getPortfolioValue() << "\n";
    cout << "Budget        : $" << node->trader.getBudget() << "\n";
    cout << "Inventory     : " << node->trader.getInventory() << " shares\n";
    cout << "Profit/Loss   : $" << node->trader.getProfitLoss() << "\n";
    cout << "Market Rank   : #" << r << " of "
         << portfolio.portfolioOST.totalNodes << " traders\n";
    cout << "Percentile    : " << p << "%\n";
    cout << "------------------------------------------\n";
}

// ===== MAIN LOOP =====
int main () {
int choice ;
while ( true ) {
displayMenu () ;
cout << " Choice : ";
cin >> choice ;
switch ( choice ) {
case 0: handleCreateProfile () ; break ;
case 1: handleBuyOrder () ; break ;
case 2: handleSellOrder () ; break ;
case 3: displayMarket () ; break ;
case 4: handleCancelBuy () ; break ;
case 5: handleCancelSell () ; break ;
case 6: handleTraderStats(); break;
case 9: {
// Cleanup : Recursively delete all allocated nodes
vector < OSTNode * > buyNodes , sellNodes , portNodes ,
tradeNodes ;
orderBook . buyOST . inorder ( orderBook . buyOST . root ,
buyNodes ) ;
orderBook . sellOST . inorder ( orderBook . sellOST . root ,
sellNodes ) ;
portfolio . portfolioOST . inorder ( portfolio .
portfolioOST . root , portNodes ) ;
portfolio . tradeHistoryOST . inorder ( portfolio .
tradeHistoryOST . root , tradeNodes ) ;
// CRITICAL : Delete from trees first
for ( auto node : buyNodes ) delete node ;
for ( auto node : sellNodes ) delete node ;
for ( auto node : portNodes ) delete node ;
for ( auto node : tradeNodes ) delete node ;
// CRITICAL : Clear the secondary map to prevent dangling pointers
// DANGER : Without this , tradersByName contains pointers to deleted OSTNodes
// If somehow code tries to access Portfolio later (e.g. , static global reuse ),
// would cause segmentation fault whendereferencing deleted memory
portfolio . clearNameMap () ;
return 0;
}
default : cout << " Invalid .\n";
}
}
}