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
 * Prompt user for bid information using console (std::in)
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
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
vector<Bid> loadBids(string csvPath) {
    cout << "Loading CSV file " << csvPath << endl;

    // Define a vector data structure to hold a collection of bids.
    vector<Bid> bids;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    try {
        // loop to read rows of a CSV file
        for (unsigned i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push this bid to the end
            bids.push_back(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
    return bids;
}

/**
 * Partition the vector of bids into two parts, low and high
 *
 * @param bids Address of the vector<Bid> instance to be partitioned
 * @param begin Beginning index to partition
 * @param end Ending index to partition
 */
int partition(vector<Bid>& bids, int begin, int end) {
	// Pick the middle value as the pivot (Hoare scheme)
	string pivotVal = bids[(begin + end) / 2].title;

	// Low and high
	int low  = begin - 1;
	int high =  end  + 1;

	while (true) {

		// Increment the low...
		do {
			++low;
		} while (bids[low].title < pivotVal);

		// And decrement the high...
		do {
			--high;
		} while (bids[high].title > pivotVal);

		// Return when the two indices 'cross'
		if (low >= high) {
			return high;
		}

		// Swap the inverted pair (where the elements < the pivot & > the pivot are in the wrong order)
		auto temp = bids[low];
		bids[low] = bids[high];
		bids[high] = temp;
	}



}

/**
 * Perform a quick sort on bid title
 * Average performance: O(n log(n))
 * Worst case performance O(n^2))
 *
 * @param bids address of the vector<Bid> instance to be sorted
 * @param begin the beginning index to sort on
 * @param end the ending index to sort on
 */
void quickSort(vector<Bid>& bids, int begin, int end) {
	if (begin < end) {
		// Partition index
		int p = partition(bids, begin, end);

		// Sort before p
		quickSort(bids, begin, p);

		// Sort after p
		quickSort(bids, p + 1, end);
	}
}

/**
 * Perform a selection sort on bid title
 * Average performance: O(n^2))
 * Worst case performance O(n^2))
 *
 * @param bid address of the vector<Bid>
 *            instance to be sorted
 */
void selectionSort(vector<Bid>& bids) {
	int size = bids.size();

	// Then successively find the minimum and put it in its correct position
	for (int i = 0; i < size - 1; ++i) {
		// Operator [] preferred over bids.at() because the bounds-checking
		// in the latter is not needed here (we know these indices are valid)

		// set the current element as the min
		int minIdx = i;

		// Find the minimum in the unsorted part (i + 1 to end)
		for (int j = i + 1; j < size; ++j)
			if (bids[j].title < bids[minIdx].title)
				// Reassign the min index if a lower value is found
				minIdx = j;

		// If the min index changed, swap the ith element with the minimum one
		if (minIdx != i) {
			auto temp = bids[minIdx];
			bids[minIdx] = bids[i];
			bids[i] = temp;
		}
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
    string csvPath;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        break;
    default:
        csvPath = "eBid_Monthly_Sales.csv";
    }

    // Define a vector to hold all the bids
    vector<Bid> bids;

    // Define a timer variable
    clock_t ticks;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Selection Sort All Bids" << endl;
        cout << "  4. Quick Sort All Bids" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            bids = loadBids(csvPath);

            cout << bids.size() << " bids read" << endl;

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 2:
            // Loop and display the bids read
            for (unsigned i = 0; i < bids.size(); ++i) {
                displayBid(bids[i]);
            }
            cout << endl;

            break;

        case 3:
        	// Initialise a timer
        	ticks = clock();

        	// selection sort the vector
        	selectionSort(bids);

        	// Calculate the elapsed time
        	ticks = clock() - ticks;

        	// Display the time taken (ticks & seconds)
        	cout << "time: " << ticks << " clock ticks\n"
        		<< "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

        	break;

        case 4:
        	// Initialise a timer
        	ticks = clock();

        	// quicksort the vector
        	quickSort(bids, 0, bids.size());

        	// Calculate the elapsed time
        	ticks = clock() - ticks;

        	// Display the time taken (ticks & seconds)
        	cout << "time: " << ticks << " clock ticks\n"
        		<< "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

        	break;

        }
    }

    cout << "Good bye." << endl;

    return 0;
}
