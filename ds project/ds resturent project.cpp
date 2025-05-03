#include <iostream>
#include <string>
#include <iomanip>
#include <stack>
#include <queue>
#include <thread>
#include <chrono>
#include <fstream>
#include <ctime> // For date and time
#include <sstream>
using namespace std;
struct Node {
    string foodName;
    int quantity;
    float price;
    int data;
    Node* prev;
    Node* next;
};
Node* headC = nullptr; // Customer list
Node* tailC = nullptr;
Node* headA = nullptr; // Admin menu list
Node* tailA = nullptr;
Node* headS = nullptr; // Sales list
Node* tailS = nullptr;
queue<string> orderQueue;
stack<Node*> adminActions;
// Function declarations
void adminLogin();
void adminMenu();
void customerMenu();
Node* createAdmin(Node*, int, string, float);
Node* createCustomer(Node*, int, int);
void displayList(Node*);
Node* deleteNode(int, Node*, Node*&);
void displayBill();
Node* addToSales(Node*, int, string, int, float);
void undoLastAdminAction();
void saveSalesDataToFile(); // Save sales data with timestamp
void displaySalesFromFile(); // Display sales from file
void admin();
void customer();
void mainMenu();
// Login function for admin
void adminLogin() {
    string username, password;
    const int maxAttempts = 3;
    while (true) {
        int attempts = 0;

        while (attempts < maxAttempts) {
            cout << "\nEnter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;

            if (username == "admin" && password == "1234") {
                cout << "\nLogin successful!\n";
                return;
            } else {
                attempts++;
                cout << "\nInvalid credentials. Attempts remaining: " << maxAttempts - attempts << "\n";
            }
        }

        if (attempts == maxAttempts) {
            cout << "\nToo many failed attempts. Please wait for 30 seconds before trying again.\n";
            this_thread::sleep_for(chrono::seconds(30));
        }
    }
}
Node* createAdmin(Node* head, int data, string foodName, float price) {
    Node* newNode = new Node();
    newNode->data = data;
    newNode->price = price;
    newNode->quantity = 0;
    newNode->foodName = foodName;
    newNode->next = nullptr;
    newNode->prev = nullptr;
    if (head == nullptr) {
        headA = tailA = newNode;
    } else {
        tailA->next = newNode;
        newNode->prev = tailA;
        tailA = newNode;
    }

    adminActions.push(newNode);
    return headA;
}
Node* createCustomer(Node* head, int data, int quantity) {
    Node* tempA = headA;
    bool found = false;

    while (tempA != nullptr) {
        if (tempA->data == data) {
            found = true;
            break;
        }
        tempA = tempA->next;
    }

    if (found) {
        Node* newNode = new Node();
        newNode->data = data;
        newNode->price = quantity * tempA->price;
        newNode->quantity = quantity;
        newNode->foodName = tempA->foodName;
        newNode->next = nullptr;
        newNode->prev = nullptr;

        if (head == nullptr) {
            headC = tailC = newNode;
        } else {
            tailC->next = newNode;
            newNode->prev = tailC;
            tailC = newNode;
        }

        orderQueue.push("Order placed for: " + newNode->foodName);
    } else {
        cout << "\n\tThis item is not present in the menu!\n";
    }

    return headC;
}
void getInput(int &choice) {
    while (true) {
        cin >> choice;
        if (cin.fail()) {
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            cout << "\n\tInvalid input. Please enter a valid number: ";
        } else {
            break; // Exit loop if input is valid
        }
    }
}

void displayList(Node* head) {
    if (head == nullptr) {
        cout << "\n\tList is empty!\n";
        return;
    }

    Node* temp = head;
    cout << "\n";
    while (temp != nullptr) {
        if (temp->quantity == 0) {
            cout << "\t" << temp->data << "\t" << temp->foodName << "\t" << fixed << setprecision(2) << temp->price << "\n";
        } else {
            cout << "\t" << temp->data << "\t" << temp->foodName << "\t" << temp->quantity << "\t" << fixed << setprecision(2) << temp->price << "\n";
        }
        temp = temp->next;
    }
    cout << "\n";
}

Node* deleteNode(int data, Node* head, Node*& tail) {
    if (head == nullptr) {
        cout << "\n\tList is empty\n";
        return head;
    }

    Node* temp = head;

    if (data == head->data) {
        head = head->next;
        if (head != nullptr) head->prev = nullptr;
        delete temp;
    } else if (data == tail->data) {
        temp = tail;
        tail = tail->prev;
        if (tail != nullptr) tail->next = nullptr;
        delete temp;
    } else {
        while (temp != nullptr && temp->data != data) {
            temp = temp->next;
        }

        if (temp != nullptr) {
            temp->prev->next = temp->next;
            if (temp->next != nullptr) temp->next->prev = temp->prev;
            delete temp;
        }
    }

    return head;
}

Node* addToSales(Node* headS, int data, string foodName, int quantity, float price) {
    Node* newNode = new Node();
    newNode->data = data;
    newNode->foodName = foodName;
    newNode->quantity = quantity;
    newNode->price = price;
    newNode->next = nullptr;
    newNode->prev = nullptr;

    if (headS == nullptr) {
        headS = newNode;
    } else {
        Node* temp = headS;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newNode;
        newNode->prev = temp;
    }

    return headS;
}
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;
// Function to trim leading and trailing spaces from a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    size_t last = str.find_last_not_of(" \t\r\n");
    if (first == string::npos || last == string::npos) {
        return "";
    }
    return str.substr(first, last - first + 1);
}
void searchSalesByDate() {
    ifstream inFile("sales_data.txt");

    if (!inFile) {
        cout << "\n\tError: Unable to open file for reading!\n";
        return;
    }
    int searchChoice;
    cout << "\n\tChoose search type:\n";
    cout << "\t1. Search by Date only (ignores time)\n";
    cout << "\t2. Search by Date and Time\n";
    cout << "\tEnter your choice (1 or 2): ";
    cin >> searchChoice;
    string searchDate;
    string searchDateTime;
    cout << "\n\tEnter the date to search (in format dd-mm-yyyy): ";
    cin.ignore(); // To ignore the newline character left by previous input
    getline(cin, searchDate);
    // If user selects search by Date and Time, get the full timestamp (date and time)
    if (searchChoice == 2) {
        cout << "\n\tEnter the exact date and time (in format dd-mm-yyyy hh:mm:ss): ";
        getline(cin, searchDateTime);
    }
    string line;
    bool found = false;
    cout << "\n\tSales results:\n";
    cout << "\t--------------------------------------\n";
    // Read through each line of the sales data
    while (getline(inFile, line)) {
        // Find the position where the date starts in the line (after "Time: ")
        size_t timePos = line.find("Time: ");
        if (timePos != string::npos) {
            // Extract the part of the line that contains the date (next 10 characters after "Time: ")
            string date = line.substr(timePos + 6, 10); // Start after "Time: " and get the next 10 characters

            if (searchChoice == 1) {
                // Compare only date part for "Date only" search
                if (date == searchDate) {
                    cout << "\t" << line << endl; // Print the matching line
                    found = true;
                }
            } else if (searchChoice == 2) {
                // Compare the full timestamp (date + time) for exact match
                if (line.find(searchDateTime) != string::npos) {
                    cout << "\t" << line << endl; // Print the matching line
                    found = true;
                }
            }
        }
    }
    if (!found) {
        cout << "\n\tNo sales data found for the specified search criteria.\n";
    } else {
        cout << "\t--------------------------------------\n";
    }
    inFile.close();
}

void saveSalesDataToFile() {
    ofstream outFile("sales_data.txt", ios::app);

    if (!outFile) {
        cout << "\n\tError: Unable to open file for writing!\n";
        return;
    }

    // Get current date and time
    time_t now = time(0);
    tm* ltm = localtime(&now);
    string timestamp = to_string(ltm->tm_mday) + "-" +
                       to_string(1 + ltm->tm_mon) + "-" +
                       to_string(1900 + ltm->tm_year) + " " +
                       to_string(ltm->tm_hour) + ":" +
                       to_string(ltm->tm_min) + ":" +
                       to_string(ltm->tm_sec);

    outFile << "\nSales Data (Time: " << timestamp << "):";

    Node* temp = headS;
    while (temp != nullptr) {
        outFile << "\t" << temp->data << "\t" << temp->foodName << "\t" 
                << temp->quantity << "\t" << fixed << setprecision(2) << temp->price << "\n";
        temp = temp->next;
    }

    outFile.close();
    cout << "\n\tSales data has been saved to sales_data.txt with the timestamp!\n";

    // Clear the sales list after saving
    headS = nullptr;
    tailS = nullptr;
}



void displaySalesFromFile() {
    ifstream inFile("sales_data.txt");

    if (!inFile) {
        cout << "\n\tError: Unable to open file for reading!\n";
        return;
    }

    string line;
    bool hasSalesData = false;

    while (getline(inFile, line)) {
        cout << "\t" << line << endl;
        hasSalesData = true;
    }

    if (!hasSalesData) {
        cout << "\n\tNo sales data found.\n";
    }

    inFile.close();
}
// Function to search sales by date and time
void displayBill() {
    if (headC == nullptr) {
        cout << "\n\tNo items in the order!\n";
        return;
    }
    displayList(headC);  // Display current order list

    Node* temp = headC;
    float totalPrice = 0;
    while (temp != nullptr) {
        totalPrice += temp->price;
        headS = addToSales(headS, temp->data, temp->foodName, temp->quantity, temp->price);
        temp = temp->next;
    }

    cout << "\tTotal price: " << fixed << setprecision(2) << totalPrice << "\n";
    
    // Save sales data to file
    saveSalesDataToFile();

    // Clear the customer list after generating the bill
    headC = nullptr;
    tailC = nullptr;
}

void undoLastAdminAction() {
    if (adminActions.empty()) {
        cout << "\n\tNo actions to undo!\n";
        return;
    }
    Node* lastAction = adminActions.top();
    adminActions.pop();
    cout << "\n\tUndoing last action: Removed " << lastAction->foodName << "\n";
    headA = deleteNode(lastAction->data, headA, tailA);
}
void adminMenu() {
    cout << "\n\t1. View total sales\n";
    cout << "\t2. Add new items to the order menu\n";
    cout << "\t3. Delete items from the order menu\n";
    cout << "\t4. Display order menu\n";
    cout << "\t5. Undo last action\n";
    cout << "\t6. Search sales by date\n";  // New option for searching sales
    cout << "\t7. Back to Main Menu\n";
    cout << "\tEnter your choice ---> ";
}
void admin() {
    adminLogin();
    while (true) {
        adminMenu();
        int choice;
        getInput(choice);
        if (choice == 7) break;
        switch (choice) {
        case 1:
            cout << "\n\tTotal Sales (from file):\n";
            displaySalesFromFile();
            break;
        case 2: {
            int data;
            string name;
            float price;
            cout << "\n\tEnter serial number of the food item: ";
            cin >> data;
            cout << "\tEnter food item name: ";
            cin.ignore();
            getline(cin, name);
            cout << "\tEnter price: ";
            cin >> price;
            headA = createAdmin(headA, data, name, price);
            cout << "\n\tNew food item added to the list!\n";
            break;
        }
        case 3: {
            int data;
            cout << "\n\tEnter serial number of the food item to delete: ";
            cin >> data;
            headA = deleteNode(data, headA, tailA);
            break;
        }
        case 4:
            cout << "\n\tOrder menu:\n";
            displayList(headA);
            break;
        case 5:
            undoLastAdminAction();
            break;
        case 6:
            searchSalesByDate();  // Calling the search function
            break;
        default:
            cout << "\n\tInvalid input. Try again.\n";
        }
    }
}
void customerMenu() {
    cout << "\n\t1. Display order menu\n";
    cout << "\t2. Place your order\n";
    cout << "\t3. View your ordered items\n";
    cout << "\t4. Delete an item from order\n";
    cout << "\t5. Display final bill\n";
    cout << "\t6. Back to Main Menu\n";
    cout << "\tEnter your choice ---> ";
}
void customer() {
    while (true) {
        customerMenu();
        int choice;
getInput(choice);
        if (choice == 6) break;
        switch (choice) {
        case 1:
        	cout << "\n\tOrder menu:\n";
            displayList(headA);
            break;		
		case 2: {
            int data, quantity;
            cout << "\n\tEnter serial number of the item to order: ";
            cin >> data;
            cout << "\tEnter quantity: ";
            cin >> quantity;
            headC = createCustomer(headC, data, quantity);
            break;
        }
        case 3:
            cout << "\n\tOrdered items:\n";
            displayList(headC);
            break;
        case 4: {
            int data;
            cout << "\n\tEnter serial number of the item to delete: ";
            cin >> data;
            headC = deleteNode(data, headC, tailC);
            break;
        }
        case 5:
            displayBill();
            break;
        default:
            cout << "\n\tInvalid input. Try again.\n";
        }
    }
}
void mainMenu() {
    cout << "\nWelcome to Restaurant Management System\n";
    cout << "\t1. Admin Section\n";
    cout << "\t2. Customer Section\n";
    cout << "\t3. Exit\n";
    cout << "\tEnter your choice ---> ";
}
int main() {
    system("color 67");
    // Initial admin menu setup
    headA = createAdmin(headA, 1, "Hot and Sour Soup", 100);
    headA = createAdmin(headA, 2, "Manchow Soup", 200);
    headA = createAdmin(headA, 3, "Manchurian Noodles", 150);
    headA = createAdmin(headA, 4, "Fried Rice", 180);
    headA = createAdmin(headA, 5, "Hakka Noodles", 80);
    while (true) {
         mainMenu();
         int choice;
    getInput(choice);
        if (choice == 3) {
            cout << "\nThank you for using the system!\n";
            break;
        }
        switch (choice) {
        case 1:
            admin();
            break;
        case 2:
            customer();
            break;
        default:
            cout << "\nInvalid input. Try again.\n";
        }
    }
    return 0;
}