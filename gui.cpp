#include "gui.h"
#include <sstream>

sf::Text GUI::makeText(string str, int size, sf::Color col, float x, float y) {
    sf::Text t;
    t.setFont(font); t.setString(str); t.setCharacterSize(size);
    t.setFillColor(col); t.setPosition(x, y);
    return t;
}

void GUI::updateInputText(InputBox& box) {
    if (box.value.empty()) {
        box.text.setString(box.placeholder);
        box.text.setFillColor(sf::Color(150,150,150));
    } else {
        box.text.setString(box.value);
        box.text.setFillColor(sf::Color::Black);
    }
}

GUI::GUI(OrderBook& ob, Portfolio& pf)
    : window(sf::VideoMode::getDesktopMode(), "Stock Market Trading Simulation", sf::Style::Fullscreen),
      orderBook(ob), portfolio(pf), activeBox(nullptr) {
    window.setFramerateLimit(30);
    
    // Lock the internal view to your original 1100x700 grid
    sf::View view(sf::FloatRect(0, 0, 1100, 700));
    window.setView(view);

    if (!font.loadFromFile("arial.ttf"))
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
            font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
    initButtons();
    initInputBoxes();
    
    // Revamped Initial Log Message
    statusMsg = "TERMINAL | System Initialized. Welcome. Create trader profiles to begin.";
}

void GUI::initInputBoxes() {
    auto makeBox = [&](InputBox& b, float x, float y, string ph) {
        b.shape.setSize({155, 28}); b.shape.setPosition(x, y);
        b.shape.setFillColor(sf::Color::White);
        b.shape.setOutlineColor(sf::Color(180,180,180));
        b.shape.setOutlineThickness(1);
        b.placeholder = ph; b.value = ""; b.active = false;
        b.text = makeText(ph, 13, sf::Color(150,150,150), x+4, y+6);
    };
    makeBox(nameBox, 20, 560, "Name");
    makeBox(roleBox, 185, 560, "Click to toggle"); 
    makeBox(priceBox, 20, 620, "Price");
    makeBox(qtyBox, 185, 620, "Quantity");
    
    // Pre-fill the Role box as a dropdown-style toggle
    roleBox.value = "buyer"; 
    updateInputText(roleBox);
}

void GUI::initButtons() {
    vector<pair<string, string>> defs = {
        {"create", "Create Profile"}, {"buy", "Buy"}, {"sell", "Sell"},
        {"canbuy", "Cancel Buy"}, {"cansell", "Cancel Sell"},
        {"stats", "Trader Stats"}, {"range", "Range Query"}, {"exit", "Exit"}
    };
    float x = 360, y = 555;
    for (auto& d : defs) {
        Button b; b.id = d.first;
        b.shape.setSize({118, 34}); b.shape.setPosition(x, y);
        b.shape.setOutlineThickness(1);
        
        // Color-Code the Exit Button to RED
        if (b.id == "exit") {
            b.shape.setFillColor(sf::Color(180, 40, 40)); 
            b.shape.setOutlineColor(sf::Color(220, 80, 80));
        } else {
            b.shape.setFillColor(sf::Color(31,56,100));
            b.shape.setOutlineColor(sf::Color(80,120,180));
        }
        
        b.label = makeText(d.second, 12, sf::Color::White, x+5, y+9);
        buttons.push_back(b); 
        x += 130;
        if (x > 800) { x = 360; y += 45; }
    }
}

void GUI::drawOrderBook() {
    sf::RectangleShape panel({350, 460}); panel.setPosition(10, 45);
    panel.setFillColor(sf::Color(12,12,28));
    panel.setOutlineColor(sf::Color(50,80,140)); panel.setOutlineThickness(1);
    window.draw(panel);
    
    window.draw(makeText("ORDER BOOK", 14, sf::Color(100,160,255), 18, 52));
    window.draw(makeText("BIDS (Buy Orders)", 11, sf::Color(80,200,80), 18, 74));
    
    window.draw(makeText("Trader", 10, sf::Color(150,150,150), 18, 88));
    window.draw(makeText("Quantity", 10, sf::Color(150,150,150), 130, 88));
    window.draw(makeText("Pricing($)", 10, sf::Color(150,150,150), 220, 88));
    
    float y = 104; int bn = orderBook.buyOST.totalNodes;
    for (int i = bn; i >= 1 && i >= bn - 6; i--) {
        OSTNode* n = orderBook.buyOST.select(orderBook.buyOST.root, i);
        if (!n) continue;
        window.draw(makeText(n->order.getTraderName(), 12, sf::Color(80,220,80), 18, y));
        window.draw(makeText(to_string(n->order.getQuantity()), 12, sf::Color(80,220,80), 130, y));
        window.draw(makeText("$" + to_string(n->key), 12, sf::Color(80,220,80), 220, y));
        y += 18;
    }
    
    y += 10;
    window.draw(makeText("ASKS (Sell Orders)", 11, sf::Color(220,80,80), 18, y)); y += 16;
    
    window.draw(makeText("Trader", 10, sf::Color(150,150,150), 18, y));
    window.draw(makeText("Quantity", 10, sf::Color(150,150,150), 130, y));
    window.draw(makeText("Pricing($)", 10, sf::Color(150,150,150), 220, y));
    y += 16;
    
    int sn = orderBook.sellOST.totalNodes;
    for (int i = 1; i <= sn && i <= 6; i++) {
        OSTNode* n = orderBook.sellOST.select(orderBook.sellOST.root, i);
        if (!n) continue;
        window.draw(makeText(n->order.getTraderName(), 12, sf::Color(220,80,80), 18, y));
        window.draw(makeText(to_string(n->order.getQuantity()), 12, sf::Color(220,80,80), 130, y));
        window.draw(makeText("$" + to_string(n->key), 12, sf::Color(220,80,80), 220, y));
        y += 18;
    }
    
    OSTNode* bid = orderBook.getBestBid(); OSTNode* ask = orderBook.getBestAsk();
    if (bid && ask) window.draw(makeText("Spread: $" + to_string(ask->key - bid->key), 12, sf::Color(255,215,0), 18, 440));
    window.draw(makeText("Buys: " + to_string(bn) + " Sells: " + to_string(sn), 11, sf::Color(160,160,160), 18, 460));
}

void GUI::drawLeaderboard() {
    sf::RectangleShape panel({360, 460}); panel.setPosition(375, 45);
    panel.setFillColor(sf::Color(12,12,28));
    panel.setOutlineColor(sf::Color(50,80,140)); panel.setOutlineThickness(1);
    window.draw(panel);
    
    window.draw(makeText("LEADERBOARD", 14, sf::Color(100,160,255), 385, 52));
    
    window.draw(makeText("Rank", 10, sf::Color(150,150,150), 385, 74));
    window.draw(makeText("Name", 10, sf::Color(150,150,150), 430, 74));
    window.draw(makeText("Role", 10, sf::Color(150,150,150), 510, 74));
    window.draw(makeText("Stock", 10, sf::Color(150,150,150), 570, 74));
    window.draw(makeText("Value", 10, sf::Color(150,150,150), 630, 74));

    int total = portfolio.portfolioOST.totalNodes; float y = 92;
    for (int i = total; i >= 1; i--) {
        OSTNode* n = portfolio.portfolioOST.select(portfolio.portfolioOST.root, i);
        if (!n) continue;
        int rank = total - i + 1;
        sf::Color col = (rank == 1) ? sf::Color(255,215,0) : sf::Color(200,200,200);
        
        window.draw(makeText("#" + to_string(rank), 12, col, 385, y));
        window.draw(makeText(n->trader.getName(), 12, col, 430, y));
        window.draw(makeText(n->trader.getRole(), 12, col, 510, y));
        window.draw(makeText(to_string(n->trader.getInventory()), 12, col, 570, y));
        window.draw(makeText("$" + to_string(n->trader.getPortfolioValue()), 12, col, 630, y));
        y += 20;
    }
}

void GUI::drawTradeHistory() {
    sf::RectangleShape panel({350, 460}); panel.setPosition(748, 45);
    panel.setFillColor(sf::Color(12,12,28));
    panel.setOutlineColor(sf::Color(50,80,140)); panel.setOutlineThickness(1);
    window.draw(panel);
    
    window.draw(makeText("TRADE HISTORY", 14, sf::Color(100,160,255), 758, 52));
    
    window.draw(makeText("ID", 10, sf::Color(150,150,150), 758, 74));
    window.draw(makeText("Buyer", 10, sf::Color(150,150,150), 800, 74));
    window.draw(makeText("Seller", 10, sf::Color(150,150,150), 880, 74));
    window.draw(makeText("Price", 10, sf::Color(150,150,150), 960, 74));
    window.draw(makeText("Qty", 10, sf::Color(150,150,150), 1030, 74));

    int total = portfolio.tradeHistoryOST.totalNodes; float y = 92;
    for (int i = total; i >= 1 && i > total - 20; i--) {
        OSTNode* t = portfolio.tradeHistoryOST.select(portfolio.tradeHistoryOST.root, i);
        if (!t) continue;
        window.draw(makeText("#" + to_string(t->trade.getTimestamp()), 11, sf::Color(180,180,180), 758, y));
        window.draw(makeText(t->trade.getBuyerName(), 11, sf::Color(80,220,80), 800, y));
        window.draw(makeText(t->trade.getSellerName(), 11, sf::Color(220,80,80), 880, y));
        window.draw(makeText("$" + to_string(t->trade.getPrice()), 11, sf::Color(255,215,0), 960, y));
        window.draw(makeText(to_string(t->trade.getQuantity()), 11, sf::Color(220,220,220), 1030, y));
        y += 18; 
    }
}

void GUI::drawStatusBar() {
    sf::RectangleShape bar({1090, 26}); bar.setPosition(5, 510);
    bar.setFillColor(sf::Color(8,8,22));
    bar.setOutlineColor(sf::Color(50,80,140)); bar.setOutlineThickness(1);
    window.draw(bar);
    
    // Check if it's an error to color it red, otherwise green
    sf::Color logColor = (statusMsg.find("[ERROR]") != string::npos) ? sf::Color(255,100,100) : sf::Color(140,220,140);
    window.draw(makeText(statusMsg, 11, logColor, 12, 516));
}

void GUI::drawInputPanel() {
    sf::RectangleShape panel({1090, 180}); panel.setPosition(5, 540);
    panel.setFillColor(sf::Color(18,18,38));
    panel.setOutlineColor(sf::Color(50,80,140)); panel.setOutlineThickness(1);
    window.draw(panel);
    
    window.draw(makeText("Name", 10, sf::Color(140,140,200), 25, 545));
    window.draw(makeText("Role (Click to Toggle)", 10, sf::Color(140,140,200), 190, 545));
    window.draw(makeText("Price / Lower Bound", 10, sf::Color(140,140,200), 25, 605));
    window.draw(makeText("Qty/ Upper Bound", 10, sf::Color(140,140,200), 190, 605));
    
    vector<InputBox*> boxes = {&nameBox, &roleBox, &priceBox, &qtyBox};
    for (auto b : boxes) {
        b->shape.setOutlineColor(b->active ? sf::Color(100,160,255) : sf::Color(180,180,180));
        window.draw(b->shape); window.draw(b->text);
    }
    for (auto& btn : buttons) { window.draw(btn.shape); window.draw(btn.label); }
}

void GUI::handleButtonClick(const string& id) {
    string name = nameBox.value, role = roleBox.value;
    int price = 0, qty = 0;
    try { price = stoi(priceBox.value); } catch(...) {}
    try { qty = stoi(qtyBox.value); } catch(...) {}

    if (id == "create") {
        if (name.empty() || role.empty() || price <= 0) {
            statusMsg = "TERMINAL | [ERROR] Fill Name and starting Amount (in Price field)."; return; 
        }
        if (portfolio.findTrader(name) != nullptr) {
            statusMsg = "TERMINAL | [ERROR] Trader '" + name + "' is already registered."; return; 
        }
        portfolio.createProfile(name, role, price);
        statusMsg = "TERMINAL | SUCCESS: Profile created for '" + name + "' (" + role + "). Initial Value = $" + to_string(price);
    
    } else if (id == "buy") {
            if (name.empty() || price <= 0 || qty <= 0) {
                statusMsg = "TERMINAL | [ERROR] Fill Name, Price, and Qty to place an order."; return; 
            }
            if (portfolio.findTrader(name) == nullptr) {
                statusMsg = "TERMINAL | [ERROR] Trader '" + name + "' is not registered."; return; 
            }
            OSTNode* t = portfolio.findTrader(name);
            if (t && t->trader.getBudget() < price * qty) {
                statusMsg = "TERMINAL | [ERROR] Insufficient budget. Trader only has $" + to_string(t->trader.getBudget()); return; 
            }
            
            orderBook.placeBuyOrder(price, qty, name);
            string buyer, seller; int tp, tq;
            int matchCount = 0;
            
            // THE FIX: While loop to sweep the order book
            while (orderBook.matchOrders(buyer, seller, tp, tq)) {
                portfolio.recordTrade(tp, tq, buyer, seller);
                portfolio.settleTradeAmounts(buyer, seller, tp, tq);
                statusMsg = "TERMINAL | MARKET MATCH: " + buyer + " bought from " + seller + " @ $" + to_string(tp) + " x" + to_string(tq);
                matchCount++;
            }
            
            if (matchCount == 0) {
                statusMsg = "TERMINAL | PENDING: Buy order placed in Order Book @ $" + to_string(price) + ". Awaiting match.";
            } else if (matchCount > 1) {
                statusMsg = "TERMINAL | MARKET SWEEP: Executed " + to_string(matchCount) + " trades to clear crossed orders.";
            }
    
    } else if (id == "sell") {
        if (name.empty() || price <= 0 || qty <= 0) {
            statusMsg = "TERMINAL | [ERROR] Fill Name, Price, and Qty to place an order."; return; 
        }
        if (portfolio.findTrader(name) == nullptr) {
            statusMsg = "TERMINAL | [ERROR] Trader '" + name + "' is not registered."; return; 
        }
        OSTNode* t = portfolio.findTrader(name);
        if (t && t->trader.getInventory() < qty) {
            statusMsg = "TERMINAL | [ERROR] Insufficient inventory. Trader only has " + to_string(t->trader.getInventory()) + " shares."; return; 
        }
        
        orderBook.placeSellOrder(price, qty, name);
        string buyer, seller; int tp, tq;
        int matchCount = 0;
        
        // THE FIX: While loop to sweep the order book
        while (orderBook.matchOrders(buyer, seller, tp, tq)) {
            portfolio.recordTrade(tp, tq, buyer, seller);
            portfolio.settleTradeAmounts(buyer, seller, tp, tq);
            statusMsg = "TERMINAL | MARKET MATCH: " + seller + " sold to " + buyer + " @ $" + to_string(tp) + " x" + to_string(tq);
            matchCount++;
        }
        
        if (matchCount == 0) {
            statusMsg = "TERMINAL | PENDING: Sell order placed in Order Book @ $" + to_string(price) + ". Awaiting match.";
        } else if (matchCount > 1) {
            statusMsg = "TERMINAL | MARKET SWEEP: Executed " + to_string(matchCount) + " trades to clear crossed orders.";
        }    
    } else if (id == "canbuy") {
        if (name.empty() || price <= 0) { statusMsg = "TERMINAL | [ERROR] Fill Name and Price to cancel."; return; }
        orderBook.cancelBuyOrder(price, name);
        statusMsg = "TERMINAL | SUCCESS: Cancelled Buy Order @ $" + to_string(price) + " for '" + name + "'";
    
    } else if (id == "cansell") {
        if (name.empty() || price <= 0) { statusMsg = "TERMINAL | [ERROR] Fill Name and Price to cancel."; return; }
        orderBook.cancelSellOrder(price, name);
        statusMsg = "TERMINAL | SUCCESS: Cancelled Sell Order @ $" + to_string(price) + " for '" + name + "'";
    
    } else if (id == "stats") {
        OSTNode* n = portfolio.findTrader(name);
        if (!n) { statusMsg = "TERMINAL | [ERROR] Trader not found: " + name; return; }
        int r = portfolio.getTraderRank(n->key);
        int p = portfolio.getTraderPercentile(n->key);
        statusMsg = "TERMINAL | STATS FOR '" + name + "': PV=$" + to_string(n->trader.getPortfolioValue())
                  + " | Rank #" + to_string(r) + " (" + to_string(p) + "th Percentile)"
                  + " | Cash=$" + to_string(n->trader.getBudget())
                  + " | Stock=" + to_string(n->trader.getInventory());
    
    } else if (id == "range") {
        if (price <= 0 || qty <= 0 || price > qty) {
            statusMsg = "TERMINAL | [ERROR] Use Price=lower, Qty=upper. Both must be positive, lower <= upper."; return; 
        }
        int count = orderBook.getOrderRangeCount(price, qty);
        statusMsg = "TERMINAL | QUERY RESULTS: Found " + to_string(count) + " active orders between $" + to_string(price) + " and $" + to_string(qty);
    
    } else if (id == "exit") {
        vector<OSTNode*> b, s, p, t;
        orderBook.buyOST.inorder(orderBook.buyOST.root, b);
        orderBook.sellOST.inorder(orderBook.sellOST.root, s);
        portfolio.portfolioOST.inorder(portfolio.portfolioOST.root, p);
        portfolio.tradeHistoryOST.inorder(portfolio.tradeHistoryOST.root, t);
        for (auto node : b) delete node; for (auto node : s) delete node;
        for (auto node : p) delete node; for (auto node : t) delete node;
        portfolio.clearNameMap(); window.close();
    }
}

void GUI::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mappedPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                sf::Vector2i pos((int)mappedPos.x, (int)mappedPos.y);
                
                activeBox = nullptr;
                vector<InputBox*> boxes = {&nameBox, &roleBox, &priceBox, &qtyBox};
                
                for (auto b : boxes) {
                    b->active = b->shape.getGlobalBounds().contains((float)pos.x, (float)pos.y);
                    if (b->active) {
                        // The Toggle Trick for the Role Box
                        if (b == &roleBox) {
                            roleBox.value = (roleBox.value == "buyer") ? "seller" : "buyer";
                            updateInputText(roleBox);
                            b->active = false; // Don't let them type in it
                        } else {
                            activeBox = b;
                        }
                    }
                }
                for (auto& btn : buttons)
                    if (btn.contains(pos)) handleButtonClick(btn.id);
            }
            
            if (event.type == sf::Event::TextEntered && activeBox) {
                if (event.text.unicode == 8) {
                    if (!activeBox->value.empty()) activeBox->value.pop_back();
                } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                    activeBox->value += (char)event.text.unicode;
                }
                updateInputText(*activeBox);
            }
        }
        
        window.clear(sf::Color(8,8,20));
        window.draw(makeText("STOCK MARKET TRADING SIMULATION", 16, sf::Color(100,160,255), 400, 14));
        drawOrderBook(); drawLeaderboard(); drawTradeHistory();
        drawStatusBar(); drawInputPanel();
        window.display();
    }
}