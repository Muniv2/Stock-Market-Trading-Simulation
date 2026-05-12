#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "orderbook.h"
#include "portfolio.h"
#include <string>
#include <vector>
using namespace std;
struct Button {
    sf::RectangleShape shape;
    sf::Text label;
    string id;
    bool contains(sf::Vector2i pos) const {
    return shape.getGlobalBounds().contains((float)pos.x, (float)pos.y);
    }
};
struct InputBox {
    sf::RectangleShape shape;
    sf::Text text;
    string value;
    string placeholder;
    bool active;
};
class GUI {
    private:
        sf::RenderWindow window;
        sf::Font font;

        int treeViewMode = 0;  // 0 = Normal UI, 1 = Sell Tree, 2 = Buy Tree    
        void drawTreeRecursive(OSTNode* node, float x, float y, float hOffset, sf::Color nodeColor);

        OrderBook& orderBook;
        Portfolio& portfolio;
        InputBox nameBox, roleBox, priceBox, qtyBox;
        InputBox* activeBox;
        vector<Button> buttons;
        string statusMsg;
        sf::Text makeText(string str, int size, sf::Color col, float x, float y);
        void initButtons();
        void initInputBoxes();
        void drawOrderBook();
        void drawLeaderboard();
        void drawTradeHistory();
        void drawInputPanel();
        void drawStatusBar();
        void handleButtonClick(const string& id);
        void updateInputText(InputBox& box);
    public:
        GUI(OrderBook& ob, Portfolio& pf);
        void run();
};