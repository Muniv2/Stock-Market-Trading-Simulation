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
    : window(sf::VideoMode(1100,700), "Stock Market Trading Simulation"),
      orderBook(ob), portfolio(pf), activeBox(nullptr) {

    window.setFramerateLimit(30);
    if (!font.loadFromFile("arial.ttf"))
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
            font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");

    initButtons();
    initInputBoxes();
    statusMsg = "Welcome. Create trader profiles to begin.";
}

void GUI::initInputBoxes() {
    auto makeBox = [&](InputBox& b, float x, float y, string ph) {
        b.shape.setSize({155,28}); b.shape.setPosition(x,y);
        b.shape.setFillColor(sf::Color::White);
        b.shape.setOutlineColor(sf::Color(180,180,180));
        b.shape.setOutlineThickness(1);
        b.placeholder=ph; b.value=""; b.active=false;
        b.text=makeText(ph,13,sf::Color(150,150,150),x+4,y+6);
    };

    makeBox(nameBox, 15, 530, "Name");
    makeBox(roleBox, 185, 530, "buyer / seller");
    makeBox(priceBox, 355, 530, "Price");
    makeBox(qtyBox, 525, 530, "Quantity");
}

void GUI::initButtons() {
    vector<pair<string,string>> defs = {
        {"create","Create Profile"},{"buy","Buy"},{"sell","Sell"},
        {"canbuy","Cancel Buy"},{"cansell","Cancel Sell"},
        {"stats","Trader Stats"},{"range","Range Query"},{"exit","Exit"},
    };

    float x=15, y=592;

    for (auto& d : defs) {
        Button b; b.id=d.first;
        b.shape.setSize({118,34}); b.shape.setPosition(x,y);
        b.shape.setFillColor(sf::Color(31,56,100));
        b.shape.setOutlineColor(sf::Color(80,120,180));
        b.shape.setOutlineThickness(1);
        b.label=makeText(d.second,12,sf::Color::White,x+5,y+9);
        buttons.push_back(b); x+=130;
        if (x>900) { x=15; y+=42; }
    }
}

void GUI::drawOrderBook() {
    sf::RectangleShape panel({350,460}); panel.setPosition(10,45);
    panel.setFillColor(sf::Color(12,12,28));
    panel.setOutlineColor(sf::Color(50,80,140)); panel.setOutlineThickness(1);
    window.draw(panel);

    window.draw(makeText("ORDER BOOK",14,sf::Color(100,160,255),18,52));
    window.draw(makeText("--- BIDS (Buy Orders) ---",11,sf::Color(80,200,80),18,78));

    float y=96; int bn=orderBook.buyOST.totalNodes;

    for (int i=bn; i>=1 && i>bn-6; i--) {
        OSTNode* n=orderBook.buyOST.select(orderBook.buyOST.root,i);
        if (!n) continue;

        string line="$"+to_string(n->key)+" qty:"+to_string(n->order.getQuantity())+"
"+n->order.getTraderName();
        window.draw(makeText(line,12,sf::Color(80,220,80),18,y));
        y+=18;
    }

    y+=10;
    window.draw(makeText("--- ASKS (Sell Orders) ---",11,sf::Color(220,80,80),18,y));
    y+=18;

    int sn=orderBook.sellOST.totalNodes;

    for (int i=1; i<=sn && i<=6; i++) {
        OSTNode* n=orderBook.sellOST.select(orderBook.sellOST.root,i);
        if (!n) continue;

        string line="$"+to_string(n->key)+" qty:"+to_string(n->order.getQuantity())+"
"+n->order.getTraderName();
        window.draw(makeText(line,12,sf::Color(220,80,80),18,y));
        y+=18;
    }

    OSTNode* bid=orderBook.getBestBid();
    OSTNode* ask=orderBook.getBestAsk();

    if (bid && ask)
        window.draw(makeText("Spread:
$"+to_string(ask->key-bid->key),12,sf::Color(255,215,0),18,440));

    window.draw(makeText("Buys:"+to_string(bn)+"
Sells:"+to_string(sn),11,sf::Color(160,160,160),18,460));
}

void GUI::drawLeaderboard() {
    sf::RectangleShape panel({360,460}); panel.setPosition(375,45);
    panel.setFillColor(sf::Color(12,12,28));
    panel.setOutlineColor(sf::Color(50,80,140)); panel.setOutlineThickness(1);
    window.draw(panel);

    window.draw(makeText("LEADERBOARD",14,sf::Color(100,160,255),385,52));

    int total=portfolio.portfolioOST.totalNodes;
    float y=76;

    for (int i=total; i>=1; i--) {
        OSTNode* n=portfolio.portfolioOST.select(portfolio.portfolioOST.root,i);
        if (!n) continue;

        int rank=total-i+1;

        string line="#"+to_string(rank)+" "+n->trader.getName()+"
$"+to_string(n->trader.getPortfolioValue())+" ("+n->trader.getRole()+")";

        sf::Color col=(rank==1)?sf::Color(255,215,0):sf::Color(200,200,200);
        window.draw(makeText(line,12,col,385,y));
        y+=20;
    }
}

void GUI::drawTradeHistory() {
    sf::RectangleShape panel({350,460}); panel.setPosition(748,45);
    panel.setFillColor(sf::Color(12,12,28));
    panel.setOutlineColor(sf::Color(50,80,140)); panel.setOutlineThickness(1);
    window.draw(panel);

    window.draw(makeText("TRADE HISTORY",14,sf::Color(100,160,255),758,52));

    int total=portfolio.tradeHistoryOST.totalNodes;
    float y=76;

    for (int i=total; i>=1 && i>total-18; i--) {
        OSTNode* t=portfolio.tradeHistoryOST.select(portfolio.tradeHistoryOST.root,i);
        if (!t) continue;

        string line="#"+to_string(t->trade.getTimestamp())+"
"+t->trade.getBuyerName()+"<-"+t->trade.getSellerName()+"
$"+to_string(t->trade.getPrice())+"x"+to_string(t->trade.getQuantity());

        window.draw(makeText(line,11,sf::Color(180,180,180),758,y));
        y+=16;
    }
}

void GUI::drawStatusBar() {
    sf::RectangleShape bar({1090,26}); bar.setPosition(5,510);
    bar.setFillColor(sf::Color(8,8,22));
    bar.setOutlineColor(sf::Color(50,80,140)); bar.setOutlineThickness(1);
    window.draw(bar);

    window.draw(makeText(statusMsg,11,sf::Color(140,220,140),12,516));
}

void GUI::drawInputPanel() {
    sf::RectangleShape panel({1090,180}); panel.setPosition(5,540);
    panel.setFillColor(sf::Color(18,18,38));
    panel.setOutlineColor(sf::Color(50,80,140)); panel.setOutlineThickness(1);
    window.draw(panel);

    window.draw(makeText("Name",10,sf::Color(140,140,200),15,520));
    window.draw(makeText("Role",10,sf::Color(140,140,200),185,520));
    window.draw(makeText("Price / Lower Bound",10,sf::Color(140,140,200),355,520));
    window.draw(makeText("Qty / Upper Bound",10,sf::Color(140,140,200),525,520));

    vector<InputBox*> boxes={&nameBox,&roleBox,&priceBox,&qtyBox};

    for (auto b : boxes) {
        b->shape.setOutlineColor(b->active ? sf::Color(100,160,255) : sf::Color(180,180,180));
        window.draw(b->shape);
        window.draw(b->text);
    }

    for (auto& btn : buttons) {
        window.draw(btn.shape);
        window.draw(btn.label);
    }
}
void GUI::handleButtonClick(const string& id) {
    string name=nameBox.value, role=roleBox.value;
    int price=0, qty=0;

    try { price=stoi(priceBox.value); } catch(...) {}
    try { qty=stoi(qtyBox.value); } catch(...) {}

    if (id=="create") {
        if (name.empty()||role.empty()||price<=0) {
            statusMsg="[ERROR] Fill Name, Role, and Amount (in Price field)."; return;
        }
        if (portfolio.findTrader(name)!=nullptr) {
            statusMsg="[ERROR] '"+name+"' already registered."; return;
        }
        if (role!="buyer"&&role!="seller") {
            statusMsg="[ERROR] Role must be buyer or seller."; return;
        }
        portfolio.createProfile(name,role,price);
        statusMsg="Profile created: "+name+" ("+role+") $"+to_string(price);

    } else if (id=="buy") {
        if (name.empty()||price<=0||qty<=0) {
            statusMsg="[ERROR] Fill Name, Price, and Qty."; return;
        }
        if (portfolio.findTrader(name)==nullptr) {
            statusMsg="[ERROR] '"+name+"' not registered."; return;
        }

        OSTNode* t=portfolio.findTrader(name);

        if (t&&t->trader.getBudget()<price*qty) {
            statusMsg="[ERROR] Insufficient budget. Have $"+to_string(t->trader.getBudget()); return;
        }

        orderBook.placeBuyOrder(price,qty,name);

        string buyer,seller; int tp,tq;

        if (orderBook.matchOrders(buyer,seller,tp,tq)) {
            portfolio.recordTrade(tp,tq,buyer,seller);
            portfolio.settleTradeAmounts(buyer,seller,tp,tq);
            statusMsg="TRADE: "+buyer+" bought from "+seller+" @ $"+to_string(tp)+" x"+to_string(tq);
        } else statusMsg="Buy order placed @ $"+to_string(price)+". No match yet.";

    } else if (id=="sell") {
        if (name.empty()||price<=0||qty<=0) {
            statusMsg="[ERROR] Fill Name, Price, and Qty."; return;
        }
        if (portfolio.findTrader(name)==nullptr) {
            statusMsg="[ERROR] '"+name+"' not registered."; return;
        }

        OSTNode* t=portfolio.findTrader(name);

        if (t&&t->trader.getInventory()<qty) {
            statusMsg="[ERROR] Insufficient inventory. Have "+to_string(t->trader.getInventory())+"
shares.";
            return;
        }

        orderBook.placeSellOrder(price,qty,name);

        string buyer,seller; int tp,tq;

        if (orderBook.matchOrders(buyer,seller,tp,tq)) {
            portfolio.recordTrade(tp,tq,buyer,seller);
            portfolio.settleTradeAmounts(buyer,seller,tp,tq);
            statusMsg="TRADE: "+buyer+" bought from "+seller+" @ $"+to_string(tp)+" x"+to_string(tq);
        } else statusMsg="Sell order placed @ $"+to_string(price)+". No match yet.";

    } else if (id=="canbuy") {
        if (name.empty()||price<=0) { statusMsg="[ERROR] Fill Name and Price."; return; }
        orderBook.cancelBuyOrder(price,name);
        statusMsg="Cancel buy requested @ $"+to_string(price)+" for "+name;

    } else if (id=="cansell") {
        if (name.empty()||price<=0) { statusMsg="[ERROR] Fill Name and Price."; return; }
        orderBook.cancelSellOrder(price,name);
        statusMsg="Cancel sell requested @ $"+to_string(price)+" for "+name;

    } else if (id=="stats") {
        OSTNode* n=portfolio.findTrader(name);

        if (!n) { statusMsg="[ERROR] Trader not found: "+name; return; }

        int r=portfolio.getTraderRank(n->key);
        int p=portfolio.getTraderPercentile(n->key);

        statusMsg=name+" $"+to_string(n->trader.getPortfolioValue())
            +" Rank #"+to_string(r)+" "+to_string(p)+"th percentile"
            +" Budget:$"+to_string(n->trader.getBudget())
            +" Inv:"+to_string(n->trader.getInventory());

    } else if (id=="range") {
        if (price<=0||qty<=0||price>qty) {
            statusMsg="[ERROR] Use Price=lower, Qty=upper. Both positive, lower<=upper."; return;
        }
        int count=orderBook.getOrderRangeCount(price,qty);
        statusMsg="Orders $"+to_string(price)+"-$"+to_string(qty)+": "+to_string(count)+" orders";

    } else if (id=="exit") {
        vector<OSTNode*> b,s,p,t;

        orderBook.buyOST.inorder(orderBook.buyOST.root,b);
        orderBook.sellOST.inorder(orderBook.sellOST.root,s);
        portfolio.portfolioOST.inorder(portfolio.portfolioOST.root,p);
        portfolio.tradeHistoryOST.inorder(portfolio.tradeHistoryOST.root,t);

        for (auto node:b) delete node;
        for (auto node:s) delete node;
        for (auto node:p) delete node;
        for (auto node:t) delete node;

        portfolio.clearNameMap();
        window.close();
    }
}

void GUI::run() {
    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type==sf::Event::Closed) window.close();

            if (event.type==sf::Event::MouseButtonPressed) {
                auto pos=sf::Mouse::getPosition(window);
                activeBox=nullptr;

                vector<InputBox*> boxes={&nameBox,&roleBox,&priceBox,&qtyBox};

                for (auto b:boxes) {
                    b->active=b->shape.getGlobalBounds().contains((float)pos.x,(float)pos.y);
                    if (b->active) activeBox=b;
                }

                for (auto& btn:buttons)
                    if (btn.contains(pos)) handleButtonClick(btn.id);
            }

            if (event.type==sf::Event::TextEntered&&activeBox) {
                if (event.text.unicode==8) {
                    if (!activeBox->value.empty()) activeBox->value.pop_back();
                } else if (event.text.unicode>=32&&event.text.unicode<128) {
                    activeBox->value+=(char)event.text.unicode;
                }
                updateInputText(*activeBox);
            }
        }

        window.clear(sf::Color(8,8,20));

        window.draw(makeText("STOCK MARKET TRADING SIMULATION | Powered by Order Statistic Trees",
            15,sf::Color(100,160,255),150,14));

        drawOrderBook();
        drawLeaderboard();
        drawTradeHistory();
        drawStatusBar();
        drawInputPanel();

        window.display();
    }
}
