#include "orderbook.h"
#include "portfolio.h"
#include "gui.h"

int main() {
    OrderBook orderBook;
    Portfolio portfolio;
    
    GUI gui(orderBook, portfolio); 
    gui.run();
    
    return 0;
}