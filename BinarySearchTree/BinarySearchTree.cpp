#include <algorithm>
#include <iostream>
#include <time.h>

// (borrowed)
#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold bid information
struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};

//============================================================================
// Overloading << for bids. Based on displayBid below.
//============================================================================
ostream& operator<<(ostream& os, const Bid& bid) {
	os << bid.bidId << ": " << bid.title << " | " << bid.amount << " | " << bid.fund << endl;
	return os;
}

/**
 * Node struct for the BTree.
 * This uses a Bid type directly.
 * Could be templatised for a more generic version.
 */
struct Node {
	Bid data;

	// Pointers to a left and right node - putting the 'bi' in 'binary' tree
	Node* left, * right;

	// Default ctor : no links left and right
	Node() : left { nullptr }, right { nullptr } {}

	// Parameterised ctor.
	Node(Bid bid) : left { nullptr }, right { nullptr } {
		data = bid;
	}

	// dtor
	~Node() {
		delete left;
		delete right;
	}
};

//============================================================================
// Binary Search Tree class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a binary search tree
 */
class BinarySearchTree {

private:
    Node* root;

    Node* AddNode(Node* node, Bid bid);
    void Traverse(Node* node);
    Node* SearchNode(Node* node, string bidId);
    Node* RemoveNode(Node* node, string bidId);

    // See the recursion in the destructor
    void DestroyRecursive(Node* node);

public:
    // Inlined default ctor
    BinarySearchTree() { root = nullptr; }

    // destructor
    virtual ~BinarySearchTree() { DestroyRecursive(root); }

    // Traverse the tree in order
    void InOrder() { Traverse(root); }

    // Insert a node
    void Insert(Bid bid) { root = AddNode(root, bid); }

    // Delete a node
    void Remove(string bidId) { root = RemoveNode(root, bidId); }

    // Search for a bid
    Bid Search(string bidId);
};

/**
 * Helper function for the destructor
 */
void BinarySearchTree::DestroyRecursive(Node* node) {
	if (node) {
		DestroyRecursive(node->left);
		DestroyRecursive(node->right);
		delete node;
	}
}

/**
 * Search for a bid
 */
Bid BinarySearchTree::Search(string bidId) {
	// Initialise a blank bid
	Bid bid;

	// Recursively search for the result
	Node* resultNode = SearchNode(root, bidId);

	// If the result is not null (= found), set its data as the return value
	if (resultNode)
		bid = resultNode->data;

    return bid;
}

/**
 * Add a bid to some node (recursive)
 *
 * @param node Current node in tree
 * @param bid Bid to be added
 */
Node* BinarySearchTree::AddNode(Node* node, Bid bid) {
	// If the current node is null, add here
    if (!node) {
    	return new Node(bid);
    }
    // If the bidId is less than the node's ID, move left
    else if (bid.bidId < node->data.bidId) {
    	node->left = AddNode(node->left, bid);
    }
    // If the bidId is greater than the node's ID, move right
	else if (bid.bidId > node->data.bidId) {
    	node->right = AddNode(node->right, bid);
    }
    return node;
}

/**
 * Private helper function for tree traversal (recursive)
 */
void BinarySearchTree::Traverse(Node* node) {
	// If the node exists
	if (node) {
		// Go left
		Traverse(node->left);
		// Output the data
		cout << node->data;
		// Go right
		Traverse(node->right);
	}
}

/**
 * Private helper function for searching a node (recursive)
 */
Node* BinarySearchTree::SearchNode(Node* node, string bidId) {

	// Base case: Return the match or null
	if (node == nullptr || node->data.bidId == bidId) {
		return node;
	}

	// If the bidId is greater than the node's data, search right
	if (bidId > node->data.bidId) {
		return SearchNode(node->right, bidId);
	}

	// Lastly (if the bidId is less than the node's data), search left
	return SearchNode(node->left, bidId);
}

/**
 * Private helper function for node deletion (recursive)
 */
Node* BinarySearchTree::RemoveNode(Node* node, string bidId) {
	if (!node)
		return node;

	// if the bidId is less than the current node's value, recurse left
	if (bidId < node->data.bidId) {
		node->left = RemoveNode(node->left, bidId);
		return node;
	}
	// if the bidId is greater than the current node's value, recurse right
	else if (bidId > node->data.bidId) {
		node->right = RemoveNode(node->right, bidId);
		return node;
	}

	// The part below is reached when the node is the one to be deleted

	// CASE 1: Deleting a leaf. Nothing much to do here...
	if (!(node->left) && !(node->right)) {
		return nullptr;
	}

	// CASE 2: Deleting nodes with one child - the child takes the parent's place
	// Empty on the left
	if (!(node->left)) {
		// Get the right child
		Node* temp = node->right;
		// Delete the parent
		delete node;
		// Return the right child
		return temp;
	}
	// Empty on the right
	else if (!(node->right)) {
		// Get the left child
		Node* temp = node->left;
		// Delete the parent
		delete node;
		// Return the left child
		return temp;
	}

	// CASE 3: Deleting a node with two children

	// The successor's parent: This node
	Node* succParent = node;

	// Its successor: Go right, and then successively left,
	// ... until there is nothing left to the left
	Node* succ = node->right;

	while (succ->left) {
		succParent = succ;
		succ = succ->left;
	}

	/* Validity of this:
	 * succ is always the left child of succParent
	 * So succ->right can be made succParent->left
	 */
	if (succParent != node) {
		succParent->left = succ->right;
	}
	/*
	 * Make succ-right succParent->right
	 */
	else {
		succParent->right = succ->right;
	}

	// Copy the successor's data to the node
	node->data = succ->data;

	// delete the successor
	delete succ;

	// return the node
	return node;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out)
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
            << bid.fund << endl;
    return;
}

/**
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
void loadBids(string csvPath, BinarySearchTree* bst) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push this bid to the end
            bst->Insert(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, bidKey;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        bidKey = "98109";
        break;
    case 3:
        csvPath = argv[1];
        bidKey = argv[2];
        break;
    default:
        csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
        bidKey = "98109";
    }

    // Define a timer variable
    clock_t ticks;

    // Define a binary search tree to hold all bids
    BinarySearchTree* bst = new BinarySearchTree;

    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Find Bid" << endl;
        cout << "  4. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            loadBids(csvPath, bst);

            //cout << bst->Size() << " bids read" << endl;

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            bst->InOrder();
            break;

        case 3:
            ticks = clock();

            bid = bst->Search(bidKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
            	cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 4:
            bst->Remove(bidKey);
            break;
        }
    }

    cout << "Good bye." << endl;

	return 0;
}
