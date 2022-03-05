#include <algorithm>
#include <climits>
#include <iostream>
#include <string> // atoi
#include <time.h>

// (borrowed)
#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// Used for this implementaton. Consider a larger size for a real scenario
const unsigned int DEFAULT_SIZE = 179;

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
	os << bid.bidId << " | " << bid.title << " | " << bid.amount << " | " << bid.fund << endl;
	return os;
}

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:

	// The size of the table. Const declared above
	unsigned tableSize = DEFAULT_SIZE;

	struct Node {
		// Bid data
		Bid bid;
		// key / ID
		unsigned key;
		// Reference to the next node
		Node* next;

		// ctor (default)
		Node() {
			key = UINT_MAX; // max possible unsigned int
			next = nullptr;
		}

		// Parametrised ctors
		// With just the bid data
		Node(Bid data) : Node() {
			bid = data;
		}

		// With bid data and a key value
		Node (Bid data, unsigned keyVal) : Node(data) {
			key = keyVal;
		}
	};

	// Store a vector of nodes
	// Hash table = array of linked lists
	vector<Node> nodes;

    unsigned int hash(int key);

public:
    HashTable(unsigned size);
    virtual ~HashTable();
    void Insert(Bid bid);
    void PrintAll();
    void Remove(string bidId);
    Bid Search(string bidId);
};

/**
 * Parameterised ctor that doubles as a default ctor
 * Pass in a different size or let it take the default value
 */
HashTable::HashTable(unsigned size = DEFAULT_SIZE) : tableSize { size } {
	// Initialise the vector to the tableSize, i.e. the number of buckets
    nodes.resize(tableSize);
}

/**
 * Destructor
 */
HashTable::~HashTable() {
	// Clear the vector
    nodes.clear();
}

/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(int key) {
	// A modular hash, because the primary key is numeric
    return key % tableSize;
}

/**
 * Insert a bid
 *
 * @param bid The bid to insert
 */
void HashTable::Insert(Bid bid) {
    unsigned key = hash(stoi(bid.bidId));

    // Get an existing node, if any.
    // [] preferred over at() because hashing ensures valid indices
    Node* existingNode = &(nodes[key]);

    // No existing node for this key
    if (existingNode == nullptr) {
    	// Initialise a new node with the bid data and key
    	Node* newNode = new Node(bid, key);

    	// Insert it at the bucket with the given key
    	nodes.insert(nodes.begin() + key, (*newNode));
    }
    // A node exists for the key
    else {
    	// If this is an open node
    	if (existingNode->key == UINT_MAX) {
    		// set its values as required
    		existingNode->key = key;
    		existingNode->bid = bid;
    		existingNode->next = nullptr;
    	}
    	// if closed
    	else {
    		// Move on to the next node
    		// Until an open node is found (this will be the last existing node in a chain)
    		while (existingNode->next != nullptr) {
    			existingNode = existingNode->next;
    		}

    		// Append a new node with the given values and key
    		existingNode->next = new Node(bid, key);
    	}
    }
}

/**
 * Print all bids
 */
void HashTable::PrintAll() {
	for (unsigned i = 0; i < tableSize; ++i) {
		// To display the first element differently (with the key)
		bool first = true;

		// Get a reference to the node at the current index
		Node* node = &(nodes[i]);

		// Traverse the entire list at the current bucket
		while (node != nullptr) {
			// If non-empty/non-erased
			if (node->key != UINT_MAX) {
				// Display the first element after the key
				if (first) {
					cout << "Key " << i << ": " << node->bid;
					first = false;
				}
				// Display the other elements after tabs
				else
					cout << "    " << i << ": " << node->bid;
				// Operator << with a Bid type automatically adds a newline (see overloading above)
			}

			// Move to the next node
			node = node->next;
		}
	}
}

/**
 * Remove a bid
 *
 * @param bidId The bid id to search for
 */
void HashTable::Remove(string bidId) {
	// Hash the ID for the key
    unsigned key = hash(stoi(bidId));

    // Get a reference to the node at the bucket
    Node* node = &(nodes[key]);

    // First node empty or deleted - do nothing
    if (node == nullptr || node->key == UINT_MAX) {
    	return;
    }

    // If deleting the first element
    if (node->key != UINT_MAX && node->bid.bidId == bidId) {
    	// Make the next node the first node
    	nodes[key] = *(node->next);

    	// Delink the current node
    	node->next = nullptr;

    	return;
    }

    Node* prevNode = node;

    // Traverse the list at the bucket
    while (node != nullptr) {
    	// Match found
    	if (node->key != UINT_MAX && node->bid.bidId == bidId) {
    		// Join the last node to the next node.
    		// So A -> B -> C becomes A -> C
    		prevNode->next = node->next;

    		// Delink the current node
    		node->next = nullptr;

    		// Reasonable to break because bidId is a primary key
    		break;
    	}

    	// prevNode tracks the last node
    	prevNode = node;
    	// Move on to the next node
    	node = node->next;
    }
}

/**
 * Search for the specified bidId
 *
 * @param bidId The bid id to search for
 */
Bid HashTable::Search(string bidId) {
    // Empty result
	Bid bid;

    // Hash the ID for the key
    unsigned key = hash(stoi(bidId));

    // Get the node at the key
    Node* node = &(nodes[key]);

    // No entry found, return the empty bid
    if (node == nullptr || node->key == UINT_MAX) {
    	return bid;
    }

    // Traverse through the list in search of a match
    while (node != nullptr) {
    	// If a match is found, return it
    	if (node->key != UINT_MAX && node->bid.bidId == bidId) {
    		return node->bid;
    	}

    	// Move to the next node
    	node = node->next;
    }

    return bid;
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
void loadBids(string csvPath, HashTable* hashTable) {
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
            hashTable->Insert(bid);
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

    // Define a hash table to hold all the bids
    HashTable* bidTable;

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
            bidTable = new HashTable();

            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            loadBids(csvPath, bidTable);

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            bidTable->PrintAll();
            break;

        case 3:
            ticks = clock();

            bid = bidTable->Search(bidKey);

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
            bidTable->Remove(bidKey);
            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
