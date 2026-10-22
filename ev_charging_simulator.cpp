/*
 * EV Charging Station Management Simulator
 * ----------------------------------------
 * A simple menu-driven C++ program that uses Object-Oriented Programming.
 *
 * Features:
 *   1. Register a vehicle          4. Start charging
 *   2. Display registered vehicles 5. Stop charging (and print bill)
 *   3. View slot availability      6. Display charging details
 *
 * OOP concepts used:
 *   - Classes and objects     (Vehicle, Car, Bike, ChargingStation)
 *   - Encapsulation           (private data + public member functions)
 *   - Constructors/Destructor (initialise data / free memory)
 *   - Inheritance             (Car and Bike inherit from Vehicle)
 *   - Polymorphism            (virtual functions getType() and getRatePerMinute())
 *
 * Charging duration is entered by the user (in minutes) when charging stops,
 * so no date/time library is needed.
 */

#include <iostream>
#include <string>
using namespace std;

const int MAX_VEHICLES = 20;  // maximum number of registered vehicles
const int NUM_SLOTS = 3;      // number of charging slots in the station
const int MAX_HISTORY = 50;   // maximum number of completed sessions stored

/* ---------------------------------------------------------------
 * Helper functions (not part of any class)
 * --------------------------------------------------------------- */

// Converts a string to UPPERCASE so "mh12ab1234" and "MH12AB1234" match.
string toUpper(string text) {
    for (int i = 0; i < (int)text.length(); i++) {
        if (text[i] >= 'a' && text[i] <= 'z') {
            text[i] = text[i] - 'a' + 'A';
        }
    }
    return text;
}

// Reads a whole line and converts it to a number.
// Returns -1 if the user typed anything that is not a positive whole number.
int readNumber() {
    string line;
    getline(cin, line);
    if (line.empty() || line.length() > 6) {
        return -1;
    }
    for (int i = 0; i < (int)line.length(); i++) {
        if (line[i] < '0' || line[i] > '9') {
            return -1;
        }
    }
    int value = 0;
    for (int i = 0; i < (int)line.length(); i++) {
        value = value * 10 + (line[i] - '0');
    }
    return value;
}

/* ---------------------------------------------------------------
 * Class Vehicle (base class)
 * Stores the common details of any electric vehicle.
 * --------------------------------------------------------------- */
class Vehicle {
private:                       // Encapsulation: data is hidden
    string regNumber;
    string ownerName;

public:
    // Constructor: sets the data when the object is created
    Vehicle(string reg, string owner) {
        regNumber = reg;
        ownerName = owner;
    }

    // Virtual destructor so the correct destructor runs for Car/Bike
    virtual ~Vehicle() {}

    // Getter functions (read-only access to private data)
    string getRegNumber() const { return regNumber; }
    string getOwnerName() const { return ownerName; }

    // Virtual functions: derived classes give their own version
    virtual string getType() const { return "Vehicle"; }
    virtual double getRatePerMinute() const { return 3.0; }
};

/* ---------------------------------------------------------------
 * Class Car (derived from Vehicle) - charges at Rs. 5 per minute
 * --------------------------------------------------------------- */
class Car : public Vehicle {
public:
    Car(string reg, string owner) : Vehicle(reg, owner) {}

    string getType() const { return "Car"; }
    double getRatePerMinute() const { return 5.0; }
};

/* ---------------------------------------------------------------
 * Class Bike (derived from Vehicle) - charges at Rs. 2 per minute
 * --------------------------------------------------------------- */
class Bike : public Vehicle {
public:
    Bike(string reg, string owner) : Vehicle(reg, owner) {}

    string getType() const { return "Bike"; }
    double getRatePerMinute() const { return 2.0; }
};

/* ---------------------------------------------------------------
 * Class ChargingStation
 * Manages the vehicles, the charging slots and the billing.
 * --------------------------------------------------------------- */
class ChargingStation {
private:
    Vehicle* vehicles[MAX_VEHICLES]; // registered vehicles (pointers allow polymorphism)
    int vehicleCount;

    int slots[NUM_SLOTS];            // slots[i] = index of vehicle, or -1 if free

    // History of completed charging sessions
    string historyReg[MAX_HISTORY];
    int historyMinutes[MAX_HISTORY];
    double historyBill[MAX_HISTORY];
    int historyCount;
    double totalEarnings;

    // Returns the index of a vehicle in the array, or -1 if not found
    int findVehicle(string reg) const {
        for (int i = 0; i < vehicleCount; i++) {
            if (vehicles[i]->getRegNumber() == reg) {
                return i;
            }
        }
        return -1;
    }

    // Returns the slot number where the vehicle is charging, or -1 if not charging
    int findSlotOfVehicle(int vehicleIndex) const {
        for (int i = 0; i < NUM_SLOTS; i++) {
            if (slots[i] == vehicleIndex) {
                return i;
            }
        }
        return -1;
    }

public:
    // Constructor: start with no vehicles, all slots free, no history
    ChargingStation() {
        vehicleCount = 0;
        historyCount = 0;
        totalEarnings = 0;
        for (int i = 0; i < NUM_SLOTS; i++) {
            slots[i] = -1;
        }
    }

    // Destructor: free the memory used by the vehicle objects
    ~ChargingStation() {
        for (int i = 0; i < vehicleCount; i++) {
            delete vehicles[i];
        }
    }

    // ---- Feature 1: register a vehicle ----
    void registerVehicle() {
        if (vehicleCount >= MAX_VEHICLES) {
            cout << "Error: Vehicle list is full.\n";
            return;
        }

        string reg, owner;
        cout << "Enter registration number (no spaces): ";
        getline(cin, reg);
        reg = toUpper(reg);

        if (reg.empty() || reg.find(' ') != string::npos) {
            cout << "Error: Invalid registration number.\n";
            return;
        }
        if (findVehicle(reg) != -1) {
            cout << "Error: Vehicle " << reg << " is already registered.\n";
            return;
        }

        cout << "Enter owner name: ";
        getline(cin, owner);
        if (owner.empty()) {
            cout << "Error: Owner name cannot be empty.\n";
            return;
        }

        cout << "Vehicle type (1 = Car, 2 = Bike): ";
        int type = readNumber();

        if (type == 1) {
            vehicles[vehicleCount] = new Car(reg, owner);
        } else if (type == 2) {
            vehicles[vehicleCount] = new Bike(reg, owner);
        } else {
            cout << "Error: Invalid vehicle type.\n";
            return;
        }
        vehicleCount++;
        cout << "Vehicle " << reg << " registered successfully.\n";
    }

    // ---- Feature 2: display all registered vehicles ----
    void displayVehicles() const {
        if (vehicleCount == 0) {
            cout << "No vehicles registered yet.\n";
            return;
        }
        cout << "\n--- Registered Vehicles ---\n";
        for (int i = 0; i < vehicleCount; i++) {
            // Polymorphism: getType() and getRatePerMinute() depend on the real object
            cout << i + 1 << ". " << vehicles[i]->getRegNumber()
                 << " | " << vehicles[i]->getOwnerName()
                 << " | " << vehicles[i]->getType()
                 << " | Rs. " << vehicles[i]->getRatePerMinute() << "/min\n";
        }
    }

    // ---- Feature 3: view slot availability ----
    void viewSlots() const {
        cout << "\n--- Charging Slots ---\n";
        for (int i = 0; i < NUM_SLOTS; i++) {
            cout << "Slot " << i + 1 << ": ";
            if (slots[i] == -1) {
                cout << "Available\n";
            } else {
                cout << "Occupied by " << vehicles[slots[i]]->getRegNumber() << "\n";
            }
        }
    }

    // ---- Feature 4: start charging ----
    void startCharging() {
        string reg;
        cout << "Enter registration number: ";
        getline(cin, reg);
        reg = toUpper(reg);

        int v = findVehicle(reg);
        if (v == -1) {
            cout << "Error: Vehicle " << reg << " is not registered.\n";
            return;
        }
        if (findSlotOfVehicle(v) != -1) {
            cout << "Error: Vehicle " << reg << " is already charging.\n";
            return;
        }

        // Find the first free slot
        for (int i = 0; i < NUM_SLOTS; i++) {
            if (slots[i] == -1) {
                slots[i] = v;
                cout << "Charging started for " << reg << " at Slot " << i + 1 << ".\n";
                return;
            }
        }
        cout << "Error: All charging slots are occupied. Please wait.\n";
    }

    // ---- Feature 5: stop charging and generate the bill ----
    void stopCharging() {
        string reg;
        cout << "Enter registration number: ";
        getline(cin, reg);
        reg = toUpper(reg);

        int v = findVehicle(reg);
        if (v == -1) {
            cout << "Error: Vehicle " << reg << " is not registered.\n";
            return;
        }
        int slot = findSlotOfVehicle(v);
        if (slot == -1) {
            cout << "Error: Vehicle " << reg << " is not currently charging.\n";
            return;
        }

        cout << "Enter charging duration in minutes: ";
        int minutes = readNumber();
        if (minutes <= 0) {
            cout << "Error: Duration must be a positive whole number.\n";
            return;
        }

        double bill = minutes * vehicles[v]->getRatePerMinute();

        // Free the slot and save the session in the history
        slots[slot] = -1;
        if (historyCount < MAX_HISTORY) {
            historyReg[historyCount] = reg;
            historyMinutes[historyCount] = minutes;
            historyBill[historyCount] = bill;
            historyCount++;
        }
        totalEarnings += bill;

        cout << "\n====== CHARGING BILL ======\n";
        cout << "Vehicle  : " << reg << " (" << vehicles[v]->getType() << ")\n";
        cout << "Owner    : " << vehicles[v]->getOwnerName() << "\n";
        cout << "Slot     : " << slot + 1 << "\n";
        cout << "Duration : " << minutes << " min\n";
        cout << "Rate     : Rs. " << vehicles[v]->getRatePerMinute() << "/min\n";
        cout << "Total    : Rs. " << bill << "\n";
        cout << "===========================\n";
    }

    // ---- Feature 6: display charging details ----
    void displayChargingDetails() const {
        cout << "\n--- Currently Charging ---\n";
        bool anyCharging = false;
        for (int i = 0; i < NUM_SLOTS; i++) {
            if (slots[i] != -1) {
                cout << "Slot " << i + 1 << ": " << vehicles[slots[i]]->getRegNumber()
                     << " (" << vehicles[slots[i]]->getOwnerName() << ")\n";
                anyCharging = true;
            }
        }
        if (!anyCharging) {
            cout << "No vehicle is charging right now.\n";
        }

        cout << "\n--- Completed Sessions ---\n";
        if (historyCount == 0) {
            cout << "No completed sessions yet.\n";
        }
        for (int i = 0; i < historyCount; i++) {
            cout << i + 1 << ". " << historyReg[i] << " | " << historyMinutes[i]
                 << " min | Rs. " << historyBill[i] << "\n";
        }
        cout << "Total earnings: Rs. " << totalEarnings << "\n";
    }
};

/* ---------------------------------------------------------------
 * main(): shows the menu and calls the right function
 * --------------------------------------------------------------- */
int main() {
    ChargingStation station;  // an object of ChargingStation
    int choice;

    cout << "=== EV Charging Station Management Simulator ===\n";

    do {
        cout << "\n----------- MENU -----------\n";
        cout << "1. Register vehicle\n";
        cout << "2. Display registered vehicles\n";
        cout << "3. View slot availability\n";
        cout << "4. Start charging\n";
        cout << "5. Stop charging and generate bill\n";
        cout << "6. Display charging details\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        choice = readNumber();

        switch (choice) {
            case 1: station.registerVehicle(); break;
            case 2: station.displayVehicles(); break;
            case 3: station.viewSlots(); break;
            case 4: station.startCharging(); break;
            case 5: station.stopCharging(); break;
            case 6: station.displayChargingDetails(); break;
            case 0: cout << "Thank you for using the simulator. Goodbye!\n"; break;
            default: cout << "Invalid choice. Please enter a number from 0 to 6.\n";
        }
    } while (choice != 0 && !cin.eof());

    return 0;
}
