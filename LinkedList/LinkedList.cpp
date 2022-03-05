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

// forward declaration
void displayBid(Bid bid);

//============================================================================
// Linked-List class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a linked-list.
 */
class LinkedList {

private:

	// Struct to hold node data
	struct BidNode {
		Bid data;
		BidNode* next;

		// default ctor
		BidNode() {
			next = nullptr;
		}

		// parameterised ctor
		BidNode(Bid bid) {
			data = bid;
			next = nullptr;
		}
	};

	/* HOUSEKEEPING VARIABLES
	 * Pointers to the head & tail
	 * An int to store the size (this makes the retrieval of the size O(1) instead of O(n) in a calculation by traversal)
	 */
	BidNode* head, * tail;
	int size;

public:
    LinkedList();
    virtual ~LinkedList();
    void Append(Bid bid);
    void Prepend(Bid bid);
    void PrintList();
    void Remove(string bidId);
    Bid Search(string bidId);
    int Size();
};

/**
 * Default constructor
 */
LinkedList::LinkedList() : head { nullptr }, tail { nullptr }, size { 0 } { // Initialisations in an initialiser list
}

/**
 * Destructor
 */
LinkedList::~LinkedList() {
}

/**
 * Append a new bid to the end of the list
 */
void LinkedList::Append(Bid bid) {
	BidNode* newNode = new BidNode(bid); // new data node

	// The first element is both the head & the tail
	if (head == nullptr) {
		head = tail = newNode;
	}

	// Else, add the new element after the tail and make it the new tail
	else {
		tail->next = newNode;
		tail = newNode;
	}

	++size; // We just added a node
}

/**
 * Prepend a new bid to the start of the list
 */
void LinkedList::Prepend(Bid bid) {
	BidNode* newNode = new BidNode(bid); // new data node

	// The first element is both the head & the tail
	if (head == nullptr) {
		head = tail = newNode;
	}

	// Else, add the new element before the head and make it the new head
	else {
		newNode->next = head;
		head = newNode;
	}

	++size; // We just added a node
}

/**
 * Simple output of all bids in the list
 */
void LinkedList::PrintList() {
	// Start at the head
	BidNode* curNode = head;

	/* TRAVERSAL LOGIC
	 * Until the end is reached
	 * Display the current node data
	 * And move on to the next node
	 */
	while (curNode != nullptr) {
		displayBid(curNode->data);
		curNode = curNode->next;
	}
}

/**
 * Remove a specified bid
 *
 * @param bidId The bid id to remove from the list
 */
void LinkedList::Remove(string bidId) {

	// Trackers: Current node (start at head), previous node (need the latter for the last else block)
	BidNode* curNode, * prevNode;
	curNode = prevNode = head;

	while (curNode != nullptr) {

		// On finding a match
		if (curNode->data.bidId == bidId) {

			// Removing the head
			if (curNode == head) {

				// Shift the head one node ahead
				BidNode* sucNode = head->next;
				head = sucNode;

				// If we just deleted the last element, the tail should be nullptr too
				if (sucNode == nullptr)
					tail = nullptr;
			}

			/* REMOVING ANY OTHER NODE
			 * Link the previous node to the next one.
			 * So removing B means A -> B -> C changes links so that the list is A -> C
			 */
			else {
				prevNode->next = curNode->next;
				delete curNode; // :'(
			}

			--size; // we just deleted a node

			// break on first deletion. Reasonable because bidID can be assumed to be a primary key
			break;
		}

		prevNode = curNode; // track the last node
		curNode = curNode->next; // move on to the next node
	}
}

/**
 * Search for the specified bidId
 *
 * @param bidId The bid id to search for
 */
Bid LinkedList::Search(string bidId) {
	BidNode* curNode = head;
	Bid result;

	// TRAVERSAL LOGIC WITH A TWIST : We set the result on a match rather than display nodes
	while (curNode != nullptr) {

		// On match
		if (curNode->data.bidId == bidId) {
			// Set the result
			result = curNode->data;

			// Break on the first match. Reasonable because bidID can be assumed to be a primary key
			break;
		}
		curNode = curNode->next; // Onward to the next node
	}

	// Return an empty bid or the first match
	return result;
}

/**
 * Returns the current size (number of elements) in the list
 */
int LinkedList::Size() {
    return size;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount
         << " | " << bid.fund << endl;
    return;
}

/**
 * Prompt user for bid information
 *
 * @return Bid struct containing the bid info
 */
Bid getBid() {
    Bid bid;

    cout << "Enter Id: ";
    cin.ignore();
    getline(cin, bid.bidId);

    cout << "Enter title: ";
    getline(cin, bid.title);

    cout << "Enter fund: ";
    cin >> bid.fund;

    cout << "Enter amount: ";
    cin.ignore();
    string strAmount;
    getline(cin, strAmount);
    bid.amount = strToDouble(strAmount, '$');

    return bid;
}

/**
 * Load a CSV file containing bids into a LinkedList
 *
 * @return a LinkedList containing all the bids read
 */
void loadBids(string csvPath, LinkedList *list) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser
    csv::Parser file = csv::Parser(csvPath);

    try {
        // loop to read rows of a CSV file
        for (unsigned i = 0; i < file.rowCount(); i++) {

            // initialize a bid using data from current row (i)
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << bid.bidId << ": " << bid.title << " | " << bid.fund << " | " << bid.amount << endl;

            // add this bid to the end
            list->Append(bid);
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
 *
 * @param arg[1] path to CSV file to load from (optional)
 * @param arg[2] the bid Id to use when searching the list (optional)
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

    clock_t ticks;

    LinkedList bidList;

    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Enter a Bid" << endl;
        cout << "  2. Load Bids" << endl;
        cout << "  3. Display All Bids" << endl;
        cout << "  4. Find Bid" << endl;
        cout << "  5. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            bid = getBid();
            bidList.Append(bid);
            // bidList.Prepend(bid); // DEBUG
            displayBid(bid);

            break;

        case 2:
            ticks = clock();

            loadBids(csvPath, &bidList);

            cout << bidList.Size() << " bids read" << endl;

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " milliseconds" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 3:
            bidList.PrintList();

            // cout << "Size: " << bidList.Size() << endl; // DEBUG

            break;

        case 4:
            ticks = clock();

            bid = bidList.Search(bidKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
            	cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 5:
            bidList.Remove(bidKey);

            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
