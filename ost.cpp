# include "ost.h"
Order::Order () : price (0) , quantity (0) , traderName ("") , timestamp(0) {}
Order::Order(int p, int q, string t, long long ts) : price(p) , quantity( q ),traderName(t), timestamp(ts) {}
int Order::getPrice () const { return price ; }
int Order::getQuantity () const { return quantity ; }
string Order::getTraderName () const { return traderName ; }
long long Order::getTimestamp () const { return timestamp ; }
void Order::setPrice (int p ) { price = p ; }
void Order::setQuantity(int q ) { quantity = q ; }
void Order::setTraderName( string t ) { traderName = t ; }
void Order::setTimestamp( long long ts ) { timestamp = ts ; }

OST::OST () : root( nullptr ) , totalNodes(0) {}
int OST::getSize( OSTNode * node ) {
if (! node ) return 0;
return node -> size ;
}
void OST :: updateSize( OSTNode * node ) {
if ( node ) node -> size = 1 + getSize( node -> left ) + getSize( node -> right ) ;
}
OSTNode * OST :: getMin( OSTNode * node ) {
if (!node) return nullptr ;
while ( node -> left ) node = node -> left ;
return node ;
}
void OST::inorder( OSTNode * node , vector < OSTNode * >& result ) {
if (!node) return ;
inorder(node->left, result ) ;
result.push_back( node ) ;
inorder(node->right, result ) ;
}
// AVL Tree Helper Methods Implementation
int OST :: getHeight ( OSTNode * node ) {
if (! node ) return 0;
return node -> height ;
}
void OST :: updateHeight ( OSTNode * node ) {
if (! node ) return ;
node -> height = 1 + max ( getHeight ( node -> left ) , getHeight ( node
-> right ) ) ;
}
int OST :: getBalanceFactor ( OSTNode * node ) {
if (! node ) return 0;
return getHeight ( node -> left ) - getHeight ( node -> right ) ;
}
void OST :: rebalance ( OSTNode *& node ) {
if (! node ) return ;
updateHeight ( node ) ;
int balance = getBalanceFactor ( node ) ;
// BALANCE FACTOR > 1: Left - heavy tree
if ( balance > 1) {
// Left - Right case : need two rotations
if ( getBalanceFactor ( node -> left ) < 0) {
rotateLeft ( node -> left ) ;
}
rotateRight ( node ) ;
}
// BALANCE FACTOR < -1: Right - heavy tree
if ( balance < -1) {
// Right - Left case : need two rotations
if ( getBalanceFactor ( node -> right ) > 0) {
rotateRight ( node -> right ) ;
}
rotateLeft ( node ) ;
}
}

// ===== AMMAR: TraderProfile Class Implementation =====
TraderProfile::TraderProfile() : name(""), role(""), budget(0),
                                 inventory(0), profitLoss(0),
                                 portfolioValue(0) {}

TraderProfile::TraderProfile(string n, string r, int b, int inv, int pl, int pv)
    : name(n), role(r), budget(b), inventory(inv), profitLoss(pl), portfolioValue(pv) {}

string TraderProfile::getName() const { return name; }
string TraderProfile::getRole() const { return role; }
int TraderProfile::getBudget() const { return budget; }
int TraderProfile::getInventory() const { return inventory; }
int TraderProfile::getProfitLoss() const { return profitLoss; }
int TraderProfile::getPortfolioValue() const { return portfolioValue; }

void TraderProfile::setName(string n) { if (!n.empty()) name = n; }
void TraderProfile::setRole(string r) { if (!r.empty()) role = r; }
void TraderProfile::setBudget(int b) { if (b >= 0) budget = b; }
void TraderProfile::setInventory(int inv) { if (inv >= 0) inventory = inv; }
void TraderProfile::setProfitLoss(int pl) { profitLoss = pl; }
void TraderProfile::setPortfolioValue(int pv) { if (pv >= 0) portfolioValue = pv; }

// ===== AMMAR: Rotation Methods Implementation =====
void OST::rotateLeft(OSTNode* node) {
    OSTNode* temp = node->right;
    node->right = temp->left;
    if (temp->left) temp->left->parent = node;
    temp->parent = node->parent;
    if (!node->parent) root = temp;
    else if (node == node->parent->left) node->parent->left = temp;
    else node->parent->right = temp;
    temp->left = node;
    node->parent = temp;
    updateSize(node);
    updateSize(temp);
    // CRITICAL: Update heights after rotation for AVL rebalancing
    updateHeight(node);
    updateHeight(temp);
}

void OST::rotateRight(OSTNode* node) {
    OSTNode* temp = node->left;
    node->left = temp->right;
    if (temp->right) temp->right->parent = node;
    temp->parent = node->parent;
    if (!node->parent) root = temp;
    else if (node == node->parent->right) node->parent->right = temp;
    else node->parent->left = temp;
    temp->right = node;
    node->parent = temp;
    updateSize(node);
    updateSize(temp);
    // CRITICAL: Update heights after rotation for AVL rebalancing
    updateHeight(node);
    updateHeight(temp);
}
