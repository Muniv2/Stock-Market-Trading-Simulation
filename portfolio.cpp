#include "portfolio.h"
#include <ctime>

// ===== Profile Management =====
Portfolio::Portfolio() : tradeCounter(0) {}

void Portfolio::createProfile(string name, string role, int amount) {
    OSTNode* node = new OSTNode(amount, TRADER_NODE);
    node->trader.setName(name);
    node->trader.setRole(role);
    node->trader.setPortfolioValue(amount);

    if (role == " buyer ") {
        node->trader.setBudget(amount);
        node->trader.setInventory(0);
    } else {
        node->trader.setInventory(amount);
        node->trader.setBudget(0);
    }

    node->trader.setProfitLoss(0);
    portfolioOST.insert(node);

    // findTrader
    tradersByName[name] = node;
}

void Portfolio::updatePortfolio(string name, int oldValue, int newValue,
                                string role, int budget, int inventory, 
                                int pnl) {
    // Delete old node BEFORE inserting new one
    // Reason : portfolioOST is keyed by portfolio value , so
    // changing wealth = changing position
    // If you insert before deleting , the old node ( ghost data )
    // remains in tree ,
    // corrupting rank () and percentile calculations with "phantom " traders
    // Find and delete old portfolio entry ( keyed by old value )
    // Search for TRADER_NODE specifically (not ORDER_NODE ) with
    // matching name and value
    OSTNode* nodeToDelete = nullptr;
    vector<OSTNode*> all;
    portfolioOST.inorder(portfolioOST.root, all);

    for (auto node : all) {
        if (node->key == oldValue && node->nodeType == TRADER_NODE &&
            node->trader.getName() == name) {
            nodeToDelete = node;
            break;
        }
    }

    // Delete old portfolio entry ( keyed by old value )
    if (nodeToDelete) {
        portfolioOST.deleteNode(nodeToDelete);
        // Remove from name - based lookup map to prevent
        // stale references
        tradersByName.erase(name);
    }

    // NOW insert new node with updated portfolio value (re -
    // positions in tree )
    // This re - ranks the trader in the wealth ordering
    OSTNode* updated = new OSTNode(newValue, TRADER_NODE);
    updated->trader.setName(name);
    updated->trader.setRole(role);
    updated->trader.setPortfolioValue(newValue);
    updated->trader.setBudget(budget);
    updated->trader.setInventory(inventory);
    updated->trader.setProfitLoss(pnl);

    portfolioOST.insert(updated);
    tradersByName[name] = updated;
}


