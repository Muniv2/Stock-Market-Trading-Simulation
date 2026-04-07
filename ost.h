# pragma once // Prevents the header from being included multiple times during compilation
#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <ctime>

using namespace std ;

// Defines types of nodes to distinguish between orders, traders, or specific trades
enum NodeType { ORDER_NODE , TRADER_NODE , TRADE_NODE };

// Global counter to give every node a unique, increasing time ID
static long long timestampCounter = 0;

// Represents a single trading order with price, quantity, and ownership details
class Order {
    private :
        int price ;
        int quantity ;
        string traderName ;
        long long timestamp ;
    public :
        Order () ;
        Order (int p , int q , string t , long long ts ) ;
        // Getters and Setters to access and modify private order data
        int getPrice () const ;
        int getQuantity () const ;
        string getTraderName () const ;
        long long getTimestamp () const ;
        void setPrice (int p ) ;
        void setQuantity (int q ) ;
        void setTraderName ( string t ) ;
        void setTimestamp ( long long ts ) ;
};

// The basic building block of the Order Statistic Tree (OST)
struct OSTNode {
    int key ;               // The value used for sorting (e.g., price)
    long long timestamp ;
    NodeType nodeType ;
    Order order ;           // The actual order data stored in this node
    OSTNode * left ;        // Pointer to left child
    OSTNode * right ;       // Pointer to right child
    OSTNode * parent ;      // Pointer to parent (useful for rank calculations)
    int size ;              // Total number of nodes in this subtree (crucial for OST)
    int height ;            // Height of the node (used for AVL balancing)

    // Constructor to initialize a new node with default values
    OSTNode(int k, NodeType t) : key(k), timestamp(timestampCounter++), nodeType(t), 
                                 left(nullptr), right(nullptr), parent(nullptr), 
                                 size(1), height(1) {}
};

inline int myMax(int a, int b) { return a > b ? a : b; }
inline int myMin(int a, int b) { return a < b ? a : b; }

// The main Order Statistic Tree class
class OST {
    public :
        OSTNode * root ;    // The top-most node of the tree
        int totalNodes ;    // Total count of elements in the entire tree
        OST () ;

        // Returns the size of a subtree
        int getSize ( OSTNode * node ) ;

        // Recalculates size based on children's sizes + 1
        void updateSize ( OSTNode * node ) ;

        // Finds the node with the smallest key in a given subtree
        OSTNode * getMin ( OSTNode * node ) ;

        // Traverses the tree in sorted order and stores nodes in a vector
        void inorder ( OSTNode * node , vector < OSTNode * >& result ) ;

        // AVL Balancing: Keeps the tree flat to ensure O(log n) speed
        int getHeight ( OSTNode * node ) ;
        void updateHeight ( OSTNode * node ) ;
        int getBalanceFactor ( OSTNode * node ) ; // Checks if left/right are heavy
        void rebalance ( OSTNode *& node ) ;      // Performs rotations if needed
        void rotateLeft(OSTNode* node);
        void rotateRight(OSTNode* node);

    };


// Trader Profile Class
class TraderProfile {
private:
    string name;
    string role;
    int budget;
    int inventory;
    int profitLoss;
    int portfolioValue;
public:
    TraderProfile();
    TraderProfile(string n, string r, int b, int inv, int pl, int pv);
    string getName() const;
    string getRole() const;
    int getBudget() const;
    int getInventory() const;
    int getProfitLoss() const;
    int getPortfolioValue() const;
    void setName(string n);
    void setRole(string r);
    void setBudget(int b);
    void setInventory(int inv);
    void setProfitLoss(int pl);
    void setPortfolioValue(int pv);
};

