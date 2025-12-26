
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <random>
#include <cctype>

using namespace std;

// ==================== DATA STRUCTURES ====================

class Train {
public:
    string trainId;
    string name;
    string source;
    string destination;
    vector<string> stations;
    vector<int> distances;
    vector<vector<int>> seats;
    int totalSeats;
    string departureTime;
    string arrivalTime;
    double farePerKm;

    Train(string id = "", string n = "", string src = "", string dest = "", int seatsCount = 0) {
        trainId = id;
        name = n;
        source = src;
        destination = dest;
        totalSeats = seatsCount;
        farePerKm = 2.5;
        if (seatsCount > 0) {
            seats.resize(10, vector<int>(seatsCount/10, 0));
        }
    }
};

class Passenger {
public:
    string name;
    int age;
    string gender;
    string contact;

    Passenger(string n = "", int a = 0, string g = "", string c = "") {
        name = n;
        age = a;
        gender = g;
        contact = c;
    }
};

class Booking {
public:
    string pnr;
    string trainId;
    string source;
    string destination;
    vector<Passenger> passengers;
    string date;
    int coach;
    vector<int> seatNumbers;
    string status;
    double fare;
    string mealPreference;

    Booking(string tid = "", string src = "", string dest = "") {
        trainId = tid;
        source = src;
        destination = dest;
        status = "Confirmed";
        fare = 0.0;
        mealPreference = "None";
    }
};

class CateringItem {
public:
    string itemId;
    string name;
    string type;
    double price;
    int quantity;

    CateringItem(string id = "", string n = "", string t = "", double p = 0.0, int q = 0) {
        itemId = id;
        name = n;
        type = t;
        price = p;
        quantity = q;
    }
};

// ==================== GLOBAL VARIABLES ====================
vector<Train> trains;
vector<Booking> bookings;
vector<CateringItem> cateringMenu;
map<string, int> pantryInventory;

// ==================== HELPER FUNCTIONS ====================

bool isNumber(const string& str) {
    if (str.empty()) return false;
    for (char c : str) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool isDouble(const string& str) {
    if (str.empty()) return false;
    bool dotFound = false;
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        if (!isdigit(c)) {
            if (c == '.' && !dotFound && i != 0 && i != str.length() - 1) {
                dotFound = true;
            } else {
                return false;
            }
        }
    }
    return true;
}

// ==================== PNR GENERATION ====================

string generatePNR(const string& travelDate = "") {
    time_t now = time(0);
    tm *ltm = localtime(&now);

    // Get raw system time
    int hour = ltm->tm_hour;
    int minute = ltm->tm_min;
    int second = ltm->tm_sec;

    // TIMEZONE ADJUSTMENT FOR INDIA (IST = UTC+5:30)
    // If system shows 13:51 but actual is 19:21, uncomment these lines:
    bool adjustForIST = true; // Set to true to convert UTC to IST

    if (adjustForIST) {
        // Add 5 hours 30 minutes for Indian Standard Time
        hour += 5;
        minute += 30;

        // Handle overflow
        if (minute >= 60) {
            minute -= 60;
            hour += 1;
        }
        if (hour >= 24) {
            hour -= 24;
        }
    }

    // Get date from system
    int pnrDay = ltm->tm_mday;
    int pnrMonth = ltm->tm_mon + 1;
    int pnrYear = ltm->tm_year + 1900;

    // Use travel date if provided (overrides system date)
    if (!travelDate.empty() && travelDate.length() == 10 && 
        travelDate[2] == '-' && travelDate[5] == '-') {

        string dayStr = travelDate.substr(0, 2);
        string monthStr = travelDate.substr(3, 2);
        string yearStr = travelDate.substr(6, 4);

        if (isNumber(dayStr) && isNumber(monthStr) && isNumber(yearStr)) {
            pnrDay = stoi(dayStr);
            pnrMonth = stoi(monthStr);
            pnrYear = stoi(yearStr);
        }
    }

    // Generate PNR: HHMMSSDDMMYYYY
    stringstream ss;
    ss << setfill('0') 
       << setw(2) << hour        // Hour (24-hour format, adjusted for IST)
       << setw(2) << minute      // Minute
       << setw(2) << second      // Second
       << setw(2) << pnrDay      // Day (from travel date or current)
       << setw(2) << pnrMonth    // Month (from travel date or current)
       << pnrYear;               // Year (from travel date or current)

    return ss.str();
}

// Function to calculate distance between two stations on a route
int calculateRouteDistance(Train* train, const string& source, const string& destination) {
    // Find positions of source and destination
    int sourcePos = -1, destPos = -1;

    // Check if source is the starting station
    if (source == train->source) {
        sourcePos = 0;
    }

    // Check if destination is the ending station
    if (destination == train->destination) {
        destPos = train->stations.size() + 1;
    }

    // Check intermediate stations
    for (int i = 0; i < train->stations.size(); i++) {
        if (train->stations[i] == source) {
            sourcePos = i + 1;
        }
        if (train->stations[i] == destination) {
            destPos = i + 1;
        }
    }

    // If both stations found and in correct order
    if (sourcePos >= 0 && destPos >= 0 && destPos > sourcePos) {
        if (destPos == (int)train->stations.size() + 1) {
            // Destination is the final station
            if (sourcePos == 0) {
                // Source is starting station
                return train->distances.back();
            } else {
                return train->distances[destPos - 2] - train->distances[sourcePos - 2];
            }
        } else {
            // Both are intermediate or source is starting
            if (sourcePos == 0) {
                return train->distances[destPos - 2];
            } else {
                return train->distances[destPos - 2] - train->distances[sourcePos - 2];
            }
        }
    }

    // Default reasonable distance for common routes
    if (source == "Mumbai" && destination == "Delhi") return 1400;
    if (source == "Delhi" && destination == "Kolkata") return 1500;
    if (source == "Chennai" && destination == "Bangalore") return 350;
    if (source == "Bangalore" && destination == "Hyderabad") return 570;
    if (source == "Delhi" && destination == "Jaipur") return 300;
    if (source == "Mumbai" && destination == "Ahmedabad") return 530;

    return 500; // Default distance
}

void saveToFile() {
    // Save trains
    ofstream trainFile("trains.dat");
    if (trainFile.is_open()) {
        for (auto &train : trains) {
            trainFile << train.trainId << "|" << train.name << "|" 
                      << train.source << "|" << train.destination << "|"
                      << train.totalSeats << "|" << train.farePerKm << "|"
                      << train.departureTime << "|" << train.arrivalTime;

            // Save stations and distances
            trainFile << "|" << train.stations.size();
            for (size_t i = 0; i < train.stations.size(); i++) {
                trainFile << "|" << train.stations[i] << "|" << train.distances[i];
            }
            trainFile << "\n";
        }
        trainFile.close();
    }

    // Save bookings
    ofstream bookingFile("bookings.dat");
    if (bookingFile.is_open()) {
        for (auto &booking : bookings) {
            bookingFile << booking.pnr << "|" << booking.trainId << "|"
                       << booking.source << "|" << booking.destination << "|"
                       << booking.date << "|" << booking.fare << "|"
                       << booking.mealPreference << "|" << booking.passengers.size();

            // Save passengers
            for (auto &passenger : booking.passengers) {
                bookingFile << "|" << passenger.name << "|" << passenger.age 
                           << "|" << passenger.gender << "|" << passenger.contact;
            }
            bookingFile << "\n";
        }
        bookingFile.close();
    }
}

void loadFromFile() {
    // Clear existing data
    trains.clear();
    bookings.clear();

    // Load trains
    ifstream trainFile("trains.dat");
    string line;
    if (trainFile.is_open()) {
        while (getline(trainFile, line)) {
            if (line.empty()) continue;

            vector<string> tokens;
            stringstream ss(line);
            string token;

            while (getline(ss, token, '|')) {
                tokens.push_back(token);
            }

            if (tokens.size() >= 8) {
                if (isNumber(tokens[4])) {
                    Train train(tokens[0], tokens[1], tokens[2], tokens[3], stoi(tokens[4]));
                    if (isDouble(tokens[5])) {
                        train.farePerKm = stod(tokens[5]);
                    }
                    train.departureTime = tokens[6];
                    train.arrivalTime = tokens[7];

                    // Load stations and distances
                    if (tokens.size() > 8 && isNumber(tokens[8])) {
                        int stationCount = stoi(tokens[8]);
                        int index = 9;
                        for (int i = 0; i < stationCount && index + 1 < tokens.size(); i++) {
                            train.stations.push_back(tokens[index]);
                            if (isNumber(tokens[index + 1])) {
                                train.distances.push_back(stoi(tokens[index + 1]));
                            }
                            index += 2;
                        }
                    }

                    trains.push_back(train);
                }
            }
        }
        trainFile.close();
    }

    // Load bookings
    ifstream bookingFile("bookings.dat");
    if (bookingFile.is_open()) {
        while (getline(bookingFile, line)) {
            if (line.empty()) continue;

            vector<string> tokens;
            stringstream ss(line);
            string token;

            while (getline(ss, token, '|')) {
                tokens.push_back(token);
            }

            if (tokens.size() >= 8) {
                Booking booking(tokens[1], tokens[2], tokens[3]);
                booking.pnr = tokens[0];
                booking.date = tokens[4];

                if (isDouble(tokens[5])) {
                    booking.fare = stod(tokens[5]);
                }

                booking.mealPreference = tokens[6];

                // Load passengers
                if (isNumber(tokens[7])) {
                    int passengerCount = stoi(tokens[7]);
                    int index = 8;
                    for (int i = 0; i < passengerCount && index + 3 < tokens.size(); i++) {
                        string name = tokens[index];
                        int age = 0;
                        if (isNumber(tokens[index + 1])) {
                            age = stoi(tokens[index + 1]);
                        }
                        string gender = tokens[index + 2];
                        string contact = tokens[index + 3];

                        booking.passengers.push_back(Passenger(name, age, gender, contact));
                        index += 4;
                    }
                }

                bookings.push_back(booking);
            }
        }
        bookingFile.close();
    }
}

// ==================== ADMIN FUNCTIONS ====================

void adminAddTrain() {
    cout << "\n=== ADD NEW TRAIN ===\n";

    string id, name, src, dest;
    int seats;

    cout << "Enter Train ID: ";
    getline(cin, id);
    cout << "Enter Train Name: ";
    getline(cin, name);
    cout << "Enter Source Station: ";
    getline(cin, src);
    cout << "Enter Destination: ";
    getline(cin, dest);

    while (true) {
        cout << "Enter Total Seats: ";
        string seatsStr;
        getline(cin, seatsStr);

        if (isNumber(seatsStr)) {
            seats = stoi(seatsStr);
            if (seats > 0 && seats <= 2000) break;
            else if (seats > 2000) cout << "Maximum 2000 seats allowed!\n";
            else cout << "Seats must be positive!\n";
        } else {
            cout << "Invalid number! Please enter a valid integer.\n";
        }
    }

    Train newTrain(id, name, src, dest, seats);

    // Add intermediate stations
    cout << "\nAdd intermediate stations (type 'done' to finish):\n";

    int stationCount = 0;
    while (true) {
        cout << "\nStation " << (stationCount + 1) << " name (or 'done' to finish): ";
        string station;
        getline(cin, station);

        // Convert to lowercase for comparison
        string stationLower = station;
        transform(stationLower.begin(), stationLower.end(), stationLower.begin(), ::tolower);

        if (stationLower == "done") {
            cout << "Finished adding stations. Total stations added: " << stationCount << endl;
            break;
        }

        if (station.empty()) {
            cout << "Station name cannot be empty! Please enter a valid name or 'done'.\n";
            continue;
        }

        // Check if station already exists
        bool duplicate = false;
        if (station == src) {
            cout << "Error: Station cannot be same as source station!\n";
            duplicate = true;
        }
        if (station == dest) {
            cout << "Error: Station cannot be same as destination station!\n";
            duplicate = true;
        }
        for (const auto& existing : newTrain.stations) {
            if (existing == station) {
                cout << "Error: Station '" << station << "' already added!\n";
                duplicate = true;
                break;
            }
        }

        if (duplicate) {
            continue;
        }

        // Get distance from source
        string distanceStr;
        int distance;
        while (true) {
            cout << "Distance from " << src << " (km, typically 50-1500): ";
            getline(cin, distanceStr);

            if (distanceStr.empty()) {
                cout << "Distance cannot be empty!\n";
                continue;
            }

            if (isNumber(distanceStr)) {
                distance = stoi(distanceStr);
                if (distance > 0 && distance <= 5000) {
                    // Check if distance is logical (should be increasing)
                    if (!newTrain.distances.empty() && distance <= newTrain.distances.back()) {
                        cout << "Error: Distance must be greater than previous station (" 
                             << newTrain.distances.back() << "km)!\n";
                        continue;
                    }
                    break;
                } else {
                    cout << "Distance must be between 1 and 5000 km!\n";
                }
            } else {
                cout << "Invalid number! Please enter a valid integer.\n";
            }
        }

        // Add station
        newTrain.stations.push_back(station);
        newTrain.distances.push_back(distance);
        stationCount++;

        cout << "✓ Station '" << station << "' added at " << distance << "km from source.\n";
    }

    cout << "\nEnter Departure Time (HH:MM, 24-hour format): ";
    getline(cin, newTrain.departureTime);

    cout << "Enter Arrival Time (HH:MM, 24-hour format): ";
    getline(cin, newTrain.arrivalTime);

    string fareStr;
    while (true) {
        cout << "Enter Fare per KM (typical: 1.5 to 4.0): Rs.";
        getline(cin, fareStr);

        if (fareStr.empty()) {
            cout << "Fare cannot be empty!\n";
            continue;
        }

        if (isDouble(fareStr)) {
            newTrain.farePerKm = stod(fareStr);
            if (newTrain.farePerKm > 0 && newTrain.farePerKm <= 10.0) break;
            else if (newTrain.farePerKm > 10.0) cout << "Fare per km is too high! Maximum Rs.10.0\n";
            else cout << "Fare must be positive!\n";
        } else {
            cout << "Invalid fare! Please enter a valid number.\n";
        }
    }

    trains.push_back(newTrain);
    cout << "\n✅ Train added successfully!\n";
    cout << "Train ID: " << newTrain.trainId << endl;
    cout << "Train Name: " << newTrain.name << endl;
    cout << "Route: " << newTrain.source;
    for (int i = 0; i < newTrain.stations.size(); i++) {
        cout << " -> " << newTrain.stations[i] << " (" << newTrain.distances[i] << "km)";
    }
    cout << " -> " << newTrain.destination << endl;

    // Calculate total distance
    int totalDistance = 0;
    if (!newTrain.distances.empty()) {
        totalDistance = newTrain.distances.back();
    }
    cout << "Total distance: " << totalDistance << " km\n";
    cout << "Total Seats: " << newTrain.totalSeats << endl;
    cout << "Departure: " << newTrain.departureTime << endl;
    cout << "Arrival: " << newTrain.arrivalTime << endl;
    cout << "Fare per KM: Rs." << fixed << setprecision(2) << newTrain.farePerKm << endl;
    cout << "Approx full journey fare: Rs." << fixed << setprecision(2) << (totalDistance * newTrain.farePerKm) << endl;

    saveToFile();
}

void adminViewTrains() {
    cout << "\n=== ALL TRAINS ===\n";
    if (trains.empty()) {
        cout << "No trains available.\n";
        return;
    }

    cout << left << setw(10) << "Train ID" 
         << setw(20) << "Name" 
         << setw(15) << "Source" 
         << setw(15) << "Destination" 
         << setw(10) << "Seats" 
         << setw(10) << "Fare/KM" 
         << endl;
    cout << string(80, '-') << endl;

    for (auto &train : trains) {
        cout << left << setw(10) << train.trainId 
             << setw(20) << train.name 
             << setw(15) << train.source 
             << setw(15) << train.destination 
             << setw(10) << train.totalSeats 
             << fixed << setprecision(2)
             << setw(10) << train.farePerKm 
             << endl;
    }
}

// ==================== PASSENGER FUNCTIONS ====================

void passengerBookTicket() {
    cout << "\n=== BOOK TICKET ===\n";

    if (trains.empty()) {
        cout << "No trains available! Please ask admin to add trains first.\n";
        return;
    }

    // Show available trains
    adminViewTrains();

    string trainId;
    cout << "\nEnter Train ID: ";
    getline(cin, trainId);

    // Find train
    Train* selectedTrain = nullptr;
    for (auto &train : trains) {
        if (train.trainId == trainId) {
            selectedTrain = &train;
            break;
        }
    }

    if (!selectedTrain) {
        cout << "Train not found!\n";
        return;
    }

    // Show available stations
    cout << "\n📋 Available Stations: " << selectedTrain->source;
    for (int i = 0; i < selectedTrain->stations.size(); i++) {
        cout << " -> " << selectedTrain->stations[i];
    }
    cout << " -> " << selectedTrain->destination << endl;

    string source, dest;
    cout << "Enter Boarding Station: ";
    getline(cin, source);
    cout << "Enter Destination Station: ";
    getline(cin, dest);

    // Validate stations
    bool validSource = (source == selectedTrain->source);
    bool validDest = (dest == selectedTrain->destination);

    for (const auto& station : selectedTrain->stations) {
        if (station == source) validSource = true;
        if (station == dest) validDest = true;
    }

    if (!validSource || !validDest) {
        cout << "Error: Invalid stations! Please check station names.\n";
        return;
    }

    if (source == dest) {
        cout << "Error: Source and destination cannot be same!\n";
        return;
    }

    // Create booking
    Booking newBooking(trainId, source, dest);

    // Get travel date
    string travelDate;
    while (true) {
        cout << "Enter travel date (DD-MM-YYYY, e.g., 15-12-2024): ";
        getline(cin, travelDate);

        // Basic date validation
        if (travelDate.length() == 10 && 
            travelDate[2] == '-' && travelDate[5] == '-') {
            string dayStr = travelDate.substr(0, 2);
            string monthStr = travelDate.substr(3, 2);
            string yearStr = travelDate.substr(6, 4);

            if (isNumber(dayStr) && isNumber(monthStr) && isNumber(yearStr)) {
                int day = stoi(dayStr);
                int month = stoi(monthStr);
                int year = stoi(yearStr);

                if (day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 2024 && year <= 2030) {
                    newBooking.date = travelDate;
                    break;
                }
            }
        }
        cout << "Invalid date format! Please use DD-MM-YYYY format.\n";
    }

    // Generate PNR automatically with travel date
    newBooking.pnr = generatePNR(newBooking.date);

    // Add passengers
    string numPassStr;
    int numPassengers;
    while (true) {
        cout << "Number of passengers (1-6): ";
        getline(cin, numPassStr);
        if (isNumber(numPassStr)) {
            numPassengers = stoi(numPassStr);
            if (numPassengers > 0 && numPassengers <= 6) break;
            else cout << "Please enter between 1 and 6 passengers.\n";
        } else {
            cout << "Invalid number!\n";
        }
    }

    for (int i = 0; i < numPassengers; i++) {
        cout << "\nPassenger " << i + 1 << ":\n";
        string name, gender, contact;
        int age;

        cout << "Name: ";
        getline(cin, name);

        string ageStr;
        while (true) {
            cout << "Age: ";
            getline(cin, ageStr);
            if (isNumber(ageStr)) {
                age = stoi(ageStr);
                if (age > 0 && age < 120) break;
                else cout << "Please enter valid age (1-119).\n";
            } else {
                cout << "Invalid age!\n";
            }
        }

        cout << "Gender (M/F/O): ";
        getline(cin, gender);
        cout << "Contact: ";
        getline(cin, contact);

        newBooking.passengers.push_back(Passenger(name, age, gender, contact));
    }

    // Calculate fare based on actual distance
    int distance = calculateRouteDistance(selectedTrain, source, dest);
    newBooking.fare = distance * selectedTrain->farePerKm * numPassengers;

    // Apply discounts for children and senior citizens
    double discount = 0.0;
    int children = 0, seniors = 0;
    for (auto &passenger : newBooking.passengers) {
        if (passenger.age < 5) {
            discount += (distance * selectedTrain->farePerKm) * 1.0; // Free for <5
            children++;
        } else if (passenger.age <= 12) {
            discount += (distance * selectedTrain->farePerKm) * 0.5; // 50% off for 5-12
            children++;
        } else if (passenger.age >= 60) {
            discount += (distance * selectedTrain->farePerKm) * 0.4; // 40% off for seniors
            seniors++;
        }
    }

    newBooking.fare -= discount;

    // Ensure fare is not negative
    if (newBooking.fare < 0) newBooking.fare = 0;

    // Meal preference
    cout << "\nSelect meal preference for all passengers:\n";
    cout << "1. Vegetarian\n2. Non-Vegetarian\n3. None\nChoice: ";
    string mealChoiceStr;
    getline(cin, mealChoiceStr);

    if (isNumber(mealChoiceStr)) {
        int mealChoice = stoi(mealChoiceStr);
        switch(mealChoice) {
            case 1: newBooking.mealPreference = "Veg"; break;
            case 2: newBooking.mealPreference = "Non-Veg"; break;
            default: newBooking.mealPreference = "None";
        }
    } else {
        newBooking.mealPreference = "None";
    }

    // Check pantry inventory
    if (newBooking.mealPreference != "None") {
        int availableMeals = 0;
        for (auto &item : cateringMenu) {
            if ((newBooking.mealPreference == "Veg" && item.type == "Veg") ||
                (newBooking.mealPreference == "Non-Veg" && item.type == "Non-Veg")) {
                availableMeals += pantryInventory[item.itemId];
            }
        }

        if (availableMeals < numPassengers) {
            cout << "\nWarning: Only " << availableMeals << " " 
                 << newBooking.mealPreference << " meals available in pantry!\n";
            cout << "Do you still want to proceed? (y/n): ";
            string choice;
            getline(cin, choice);
            if (choice == "n" || choice == "N") {
                newBooking.mealPreference = "None";
            }
        }
    }

    bookings.push_back(newBooking);

    cout << "\n=== BOOKING CONFIRMED ===\n";
    cout << " PNR: " << newBooking.pnr << endl;
    cout << " Train: " << selectedTrain->name << " (" << trainId << ")\n";
    cout << " Route: " << source << " to " << dest << endl;
    cout << " Travel Date: " << newBooking.date << endl;
    cout << " Passengers: " << numPassengers;
    if (children > 0) cout << " (" << children << " child" << (children > 1 ? "ren" : "") << ")";
    if (seniors > 0) cout << " (" << seniors << " senior" << (seniors > 1 ? "s" : "") << ")";
    cout << endl;
    cout << " Distance: " << distance << " km\n";
    cout << " Fare per km: Rs." << fixed << setprecision(2) << selectedTrain->farePerKm << endl;
    cout << " Total Fare: Rs." << fixed << setprecision(2) << newBooking.fare << endl;
    if (discount > 0) {
        cout << " Discount applied: Rs." << fixed << setprecision(2) << discount << endl;
    }
    cout << "  Meal Preference: " << newBooking.mealPreference << endl;
    cout << "\n⚠️  IMPORTANT: Your PNR " << newBooking.pnr << " is for travel on " 
         << newBooking.date << ". Keep it safe!\n";

    saveToFile();
}

void passengerViewReservations() {
    cout << "\n=== VIEW RESERVATIONS ===\n";

    if (bookings.empty()) {
        cout << "No reservations found.\n";
        return;
    }

    string pnr;
    cout << "Enter PNR Number: ";
    getline(cin, pnr);

    bool found = false;
    for (auto &booking : bookings) {
        if (booking.pnr == pnr) {
            found = true;
            cout << "\n=== RESERVATION DETAILS ===\n";
            cout << " PNR: " << booking.pnr << endl;
            cout << " Train ID: " << booking.trainId << endl;
            cout << " Route: " << booking.source << " to " << booking.destination << endl;
            cout << " Travel Date: " << booking.date << endl;
            cout << " Fare: Rs." << fixed << setprecision(2) << booking.fare << endl;
            cout << " Status: " << booking.status << endl;
            cout << "  Meal: " << booking.mealPreference << endl;

            cout << "\n Passengers (" << booking.passengers.size() << "):\n";
            for (int i = 0; i < booking.passengers.size(); i++) {
                cout << i+1 << ". " << booking.passengers[i].name 
                     << " (" << booking.passengers[i].age << " years, " 
                     << booking.passengers[i].gender << ") - "
                     << booking.passengers[i].contact << endl;
            }
            break;
        }
    }

    if (!found) {
        cout << "No reservation found with PNR: " << pnr << endl;
    }
}

// ==================== FEATURE 4: CHEAPER ALTERNATIVE ROUTES ====================

void suggestCheaperRoutes() {
    cout << "\n=== CHEAPER ALTERNATIVE ROUTES ===\n";

    if (trains.empty()) {
        cout << "No trains available.\n";
        return;
    }

    string source, dest;
    cout << "Enter Source Station: ";
    getline(cin, source);
    cout << "Enter Destination Station: ";
    getline(cin, dest);

    if (source == dest) {
        cout << "Source and destination cannot be same!\n";
        return;
    }

    vector<pair<double, Train*>> alternatives;

    for (auto &train : trains) {
        bool hasSource = false;
        bool hasDest = false;

        // Check source
        if (train.source == source) hasSource = true;
        if (train.destination == dest) hasDest = true;

        // Check intermediate stations
        for (const auto& station : train.stations) {
            if (station == source) hasSource = true;
            if (station == dest) hasDest = true;
        }

        if (hasSource && hasDest) {
            // Calculate actual distance
            int distance = calculateRouteDistance(&train, source, dest);
            double fare = distance * train.farePerKm;
            alternatives.push_back({fare, &train});
        }
    }

    if (alternatives.empty()) {
        cout << "\n❌ No direct routes found between " << source << " and " << dest << ".\n";
        cout << " Try breaking journey into segments!\n";
    } else {
        // Sort by fare
        sort(alternatives.begin(), alternatives.end());

        cout << "\n=== AVAILABLE ROUTES (Sorted by Fare) ===\n";
        cout << left << setw(10) << "Train ID" 
             << setw(20) << "Train Name" 
             << setw(15) << "Source" 
             << setw(15) << "Destination" 
             << setw(10) << "Distance" 
             << setw(15) << "Fare" 
             << endl;
        cout << string(85, '-') << endl;

        for (auto &alt : alternatives) {
            int distance = calculateRouteDistance(alt.second, source, dest);
            cout << left << setw(10) << alt.second->trainId 
                 << setw(20) << alt.second->name 
                 << setw(15) << alt.second->source 
                 << setw(15) << alt.second->destination 
                 << setw(10) << distance << "km"
                 << "Rs." << setw(12) << fixed << setprecision(2) << alt.first 
                 << endl;
        }

        if (alternatives.size() > 1) {
            double cheapest = alternatives[0].first;
            double expensive = alternatives.back().first;
            double savings = expensive - cheapest;
            double savingsPercent = (savings / expensive) * 100;

            cout << "\n You can save up to Rs." << fixed << setprecision(2) << savings 
                 << " (" << fixed << setprecision(1) << savingsPercent << "%) by choosing "
                 << alternatives[0].second->name << "!\n";
        }
    }
}

// ==================== FEATURE 5 & 7: CATERING & INVENTORY ====================

void initializeCateringMenu() {
    // Clear existing
    cateringMenu.clear();
    pantryInventory.clear();

    // Add sample items with realistic prices
    cateringMenu.push_back(CateringItem("VEG001", "Vegetable Thali", "Veg", 120.0, 50));
    cateringMenu.push_back(CateringItem("VEG002", "Vegetable Biryani", "Veg", 150.0, 30));
    cateringMenu.push_back(CateringItem("VEG003", "Paneer Masala", "Veg", 180.0, 40));
    cateringMenu.push_back(CateringItem("VEG004", "Dal Rice", "Veg", 80.0, 60));
    cateringMenu.push_back(CateringItem("NV001", "Chicken Biryani", "Non-Veg", 200.0, 35));
    cateringMenu.push_back(CateringItem("NV002", "Egg Curry", "Non-Veg", 100.0, 60));
    cateringMenu.push_back(CateringItem("NV003", "Mutton Curry", "Non-Veg", 250.0, 25));
    cateringMenu.push_back(CateringItem("BEV001", "Coffee", "Beverage", 30.0, 100));
    cateringMenu.push_back(CateringItem("BEV002", "Tea", "Beverage", 20.0, 100));
    cateringMenu.push_back(CateringItem("SNK001", "Chips", "Snack", 40.0, 80));

    // Initialize inventory
    for (auto &item : cateringMenu) {
        pantryInventory[item.itemId] = item.quantity;
    }
}

void viewCateringMenu() {
    cout << "\n=== CATERING MENU ===\n";
    if (cateringMenu.empty()) {
        cout << "Menu is empty. Please initialize catering menu from admin.\n";
        return;
    }

    cout << left << setw(10) << "Item ID" 
         << setw(25) << "Item Name" 
         << setw(15) << "Type" 
         << setw(10) << "Price" 
         << setw(10) << "Available" 
         << endl;
    cout << string(70, '-') << endl;

    for (auto &item : cateringMenu) {
        cout << left << setw(10) << item.itemId 
             << setw(25) << item.name 
             << setw(15) << item.type 
             << "Rs." << setw(7) << fixed << setprecision(2) << item.price 
             << setw(10) << pantryInventory[item.itemId] 
             << endl;
    }
}

void orderCatering() {
    cout << "\n=== ORDER CATERING ===\n";

    if (bookings.empty()) {
        cout << "No bookings found. Please book a ticket first.\n";
        return;
    }

    string pnr;
    cout << "Enter PNR Number: ";
    getline(cin, pnr);

    // Find booking
    Booking* booking = nullptr;
    for (auto &b : bookings) {
        if (b.pnr == pnr) {
            booking = &b;
            break;
        }
    }

    if (!booking) {
        cout << "Booking not found!\n";
        return;
    }

    cout << "Booking found: " << booking->trainId << " (" 
         << booking->source << " to " << booking->destination << ")\n";

    viewCateringMenu();

    string itemId;
    string qtyStr;
    int quantity;

    cout << "\nEnter Item ID to order: ";
    getline(cin, itemId);

    // Validate item ID
    bool validItem = false;
    CateringItem* selectedItem = nullptr;
    for (auto &item : cateringMenu) {
        if (item.itemId == itemId) {
            validItem = true;
            selectedItem = &item;
            break;
        }
    }

    if (!validItem) {
        cout << "Invalid Item ID!\n";
        return;
    }

    while (true) {
        cout << "Enter Quantity (max " << pantryInventory[itemId] << "): ";
        getline(cin, qtyStr);
        if (isNumber(qtyStr)) {
            quantity = stoi(qtyStr);
            if (quantity > 0 && quantity <= pantryInventory[itemId]) break;
            else if (quantity > pantryInventory[itemId]) {
                cout << "Only " << pantryInventory[itemId] << " available!\n";
            } else {
                cout << "Quantity must be positive!\n";
            }
        } else {
            cout << "Invalid quantity!\n";
        }
    }

    // Process order
    pantryInventory[itemId] -= quantity;
    selectedItem->quantity = pantryInventory[itemId];

    double total = selectedItem->price * quantity;

    cout << "\n✅ ORDER CONFIRMED\n";
    cout << "================\n";
    cout << "Item: " << selectedItem->name << " (" << selectedItem->type << ")\n";
    cout << "Quantity: " << quantity << "\n";
    cout << "Price per item: Rs." << fixed << setprecision(2) << selectedItem->price << "\n";
    cout << "Total amount: Rs." << fixed << setprecision(2) << total << "\n";
    cout << "Delivery: Will be served during the journey\n";

    // Update booking meal preference
    if (booking->mealPreference == "None") {
        booking->mealPreference = selectedItem->type + " (" + to_string(quantity) + "x " + selectedItem->name + ")";
    } else {
        booking->mealPreference += ", " + selectedItem->type + " (" + to_string(quantity) + "x " + selectedItem->name + ")";
    }

    cout << "\n📝 Note: Your meal preference has been updated.\n";

    saveToFile();
}

void updateInventory() {
    cout << "\n=== UPDATE CATERING INVENTORY ===\n";

    viewCateringMenu();

    string itemId;
    string qtyStr;
    int quantity;

    cout << "\nEnter Item ID to update: ";
    getline(cin, itemId);

    // Validate item ID
    bool validItem = false;
    CateringItem* selectedItem = nullptr;
    for (auto &item : cateringMenu) {
        if (item.itemId == itemId) {
            validItem = true;
            selectedItem = &item;
            break;
        }
    }

    if (!validItem) {
        cout << "Invalid Item ID!\n";
        return;
    }

    cout << "Current quantity: " << pantryInventory[itemId] << endl;

    while (true) {
        cout << "Enter quantity to add (use negative to remove): ";
        getline(cin, qtyStr);
        if (isNumber(qtyStr)) {
            quantity = stoi(qtyStr);
            break;
        } else {
            cout << "Invalid quantity!\n";
        }
    }

    int newQuantity = pantryInventory[itemId] + quantity;
    if (newQuantity < 0) {
        cout << "Warning: Quantity cannot be negative! Setting to 0.\n";
        newQuantity = 0;
    }

    pantryInventory[itemId] = newQuantity;
    selectedItem->quantity = newQuantity;

    cout << "\n✅ Inventory updated successfully!\n";
    cout << "Item: " << selectedItem->name << endl;
    cout << "New quantity: " << newQuantity << endl;
}

// ==================== FEATURE 6: CANCELLATION PREDICTION ====================

double predictCancellationProbability(const Booking& booking) {
    // Simple prediction algorithm
    double probability = 0.0;

    // Factor 1: Days until travel
    int travelDay = 1, travelMonth = 1, travelYear = 2024;
    if (booking.date.length() == 10 && booking.date[2] == '-' && booking.date[5] == '-') {
        string dayStr = booking.date.substr(0, 2);
        string monthStr = booking.date.substr(3, 2);
        string yearStr = booking.date.substr(6, 4);

        if (isNumber(dayStr) && isNumber(monthStr) && isNumber(yearStr)) {
            travelDay = stoi(dayStr);
            travelMonth = stoi(monthStr);
            travelYear = stoi(yearStr);
        }
    }

    // Get current date
    time_t now = time(0);
    tm* currentTime = localtime(&now);
    int currentDay = currentTime->tm_mday;
    int currentMonth = currentTime->tm_mon + 1;
    int currentYear = currentTime->tm_year + 1900;

    // Calculate days difference (simplified)
    int daysDiff = 0;
    if (travelYear == currentYear) {
        daysDiff = (travelMonth - currentMonth) * 30 + (travelDay - currentDay);
    } else {
        daysDiff = (travelYear - currentYear) * 365 + (travelMonth - currentMonth) * 30 + (travelDay - currentDay);
    }

    if (daysDiff < 0) daysDiff = 0;

    if (daysDiff == 0) probability += 0.1;
    else if (daysDiff <= 2) probability += 0.15;
    else if (daysDiff <= 7) probability += 0.25;
    else if (daysDiff <= 30) probability += 0.35;
    else probability += 0.45;

    // Factor 2: Group size
    int groupSize = booking.passengers.size();
    if (groupSize == 1) probability += 0.10;
    else if (groupSize == 2) probability += 0.15;
    else if (groupSize <= 4) probability += 0.20;
    else probability += 0.25;

    // Factor 3: Fare amount
    double farePerPerson = booking.fare / max(1, groupSize);
    if (farePerPerson > 5000) probability += 0.20;
    else if (farePerPerson > 2000) probability += 0.15;
    else probability += 0.10;

    // Factor 4: Meal preference
    if (booking.mealPreference != "None") probability -= 0.15;

    // Factor 5: Random factor
    probability += (rand() % 20 - 10) / 100.0;

    // Ensure between 0 and 1
    probability = max(0.0, min(1.0, probability));

    return probability * 100;
}

void viewCancellationPrediction() {
    cout << "\n=== CANCELLATION PREDICTION ===\n";

    if (bookings.empty()) {
        cout << "No bookings found.\n";
        return;
    }

    string pnr;
    cout << "Enter PNR Number: ";
    getline(cin, pnr);

    for (auto &booking : bookings) {
        if (booking.pnr == pnr) {
            double probability = predictCancellationProbability(booking);

            cout << "\n=== PREDICTION RESULTS ===\n";
            cout << "PNR: " << booking.pnr << endl;
            cout << "Train: " << booking.trainId << endl;
            cout << "Passengers: " << booking.passengers.size() << endl;
            cout << "Total Fare: Rs." << fixed << setprecision(2) << booking.fare << endl;
            cout << "Meal Preference: " << booking.mealPreference << endl;

            cout << "\n Cancellation Probability: " << fixed << setprecision(1) 
                 << probability << "%" << endl;

            cout << "\nRisk Level: ";
            if (probability < 20) {
                cout << "🟢 VERY LOW (Highly likely to travel)\n";
            } else if (probability < 40) {
                cout << "🟡 LOW (Likely to travel)\n";
            } else if (probability < 60) {
                cout << "🟠 MEDIUM (Moderate cancellation risk)\n";
            } else if (probability < 80) {
                cout << "🔴 HIGH (Consider cancellation)\n";
            } else {
                cout << "🔴🔴 VERY HIGH (Very likely to cancel)\n";
            }

            // Suggestions
            cout << "\n💡 Recommendations:\n";
            if (probability > 50) {
                cout << "1. Consider flexible ticket options\n";
                cout << "2. Set cancellation reminders\n";
                cout << "3. Check refund policy (usually 50-90% refund)\n";
                cout << "4. Consider travel insurance\n";
                cout << "5. Monitor train status regularly\n";
            } else {
                cout << "1. You're likely to travel - prepare for journey\n";
                cout << "2. Arrive at station 1 hour before departure\n";
                cout << "3. Keep PNR and ID proof handy\n";
                cout << "4. Check platform number before boarding\n";
            }

            // Show confidence factors
            cout << "\n📈 Key Factors Considered:\n";
            cout << "- Group size: " << booking.passengers.size() << " passengers\n";
            cout << "- Total fare: Rs." << fixed << setprecision(2) << booking.fare << endl;
            cout << "- Meal preference: " << (booking.mealPreference != "None" ? "Set" : "Not set") << endl;
            cout << "- Travel date: " << booking.date << endl;

            return;
        }
    }

    cout << "❌ Booking not found!\n";
}

// ==================== MAIN MENU ====================

void adminMenu() {
    int choice;
    do {
        cout << "\n=== ADMIN MENU ===\n";
        cout << "1. Add New Train\n";
        cout << "2. View All Trains\n";
        cout << "3. Update Catering Inventory\n";
        cout << "4. View All Bookings\n";
        cout << "5. Reset Catering Menu\n";
        cout << "6. View System Stats\n";
        cout << "7. Back to Main Menu\n";
        cout << "Choice: ";

        string choiceStr;
        getline(cin, choiceStr);

        if (!isNumber(choiceStr)) {
            cout << "Invalid choice! Please enter a number.\n";
            continue;
        }

        choice = stoi(choiceStr);

        switch(choice) {
            case 1: adminAddTrain(); break;
            case 2: adminViewTrains(); break;
            case 3: updateInventory(); break;
            case 4: 
                cout << "\n=== ALL BOOKINGS ===\n";
                if (bookings.empty()) {
                    cout << "No bookings yet.\n";
                } else {
                    cout << left << setw(15) << "PNR" 
                         << setw(10) << "Train ID" 
                         << setw(20) << "Route" 
                         << setw(10) << "Passengers" 
                         << setw(15) << "Fare" 
                         << endl;
                    cout << string(70, '-') << endl;

                    for (auto &b : bookings) {
                        string route = b.source + "-" + b.destination;
                        cout << left << setw(15) << b.pnr 
                             << setw(10) << b.trainId 
                             << setw(20) << route 
                             << setw(10) << b.passengers.size() 
                             << "Rs." << setw(12) << fixed << setprecision(2) << b.fare 
                             << endl;
                    }
                    cout << "\nTotal bookings: " << bookings.size() << endl;
                }
                break;
            case 5: 
                initializeCateringMenu();
                cout << "✅ Catering menu reset to default.\n";
                break;
            case 6:
                cout << "\n=== SYSTEM STATISTICS ===\n";
                cout << "Trains in system: " << trains.size() << endl;
                cout << "Total bookings: " << bookings.size() << endl;
                cout << "Catering items: " << cateringMenu.size() << endl;
                cout << "Data files: trains.dat, bookings.dat\n";
                break;
            case 7: break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 7);
}

void passengerMenu() {
    int choice;
    do {
        cout << "\n=== PASSENGER MENU ===\n";
        cout << "1. Book Ticket\n";
        cout << "2. View Reservation\n";
        cout << "3. Suggest Cheaper Routes\n";
        cout << "4. View Catering Menu\n";
        cout << "5. Order Catering\n";
        cout << "6. Check Cancellation Probability\n";
        cout << "7. Back to Main Menu\n";
        cout << "Choice: ";

        string choiceStr;
        getline(cin, choiceStr);

        if (!isNumber(choiceStr)) {
            cout << "Invalid choice! Please enter a number.\n";
            continue;
        }

        choice = stoi(choiceStr);

        switch(choice) {
            case 1: passengerBookTicket(); break;
            case 2: passengerViewReservations(); break;
            case 3: suggestCheaperRoutes(); break;
            case 4: viewCateringMenu(); break;
            case 5: orderCatering(); break;
            case 6: viewCancellationPrediction(); break;
            case 7: break;
            default: cout << "Invalid choice!\n";
        }
    } while (choice != 7);
}

int main() {
    // Initialize data
    cout << "=======================================\n";
    cout << "   RAILWAY TICKET MANAGEMENT SYSTEM    \n";
    cout << "=======================================\n";
    cout << "Loading system data...\n";

    loadFromFile();
    initializeCateringMenu();

    srand(time(0));

    cout << "✅ System initialized successfully!\n";
    cout << "Trains loaded: " << trains.size() << endl;
    cout << "Bookings loaded: " << bookings.size() << endl;
    cout << "Catering items: " << cateringMenu.size() << endl;
    cout << "=======================================\n";

    int mainChoice;
    do {
        cout << "\n=== MAIN MENU ===\n";
        cout << "1. Admin Login\n";
        cout << "2. Passenger Portal\n";
        cout << "3. View System Info\n";
        cout << "4. Exit\n";
        cout << "Choice: ";

        string choiceStr;
        getline(cin, choiceStr);

        if (!isNumber(choiceStr)) {
            cout << "Invalid choice! Please enter a number.\n";
            continue;
        }

        mainChoice = stoi(choiceStr);

        switch(mainChoice) {
            case 1: adminMenu(); break;
            case 2: passengerMenu(); break;
            case 3:
                cout << "\n=== SYSTEM INFORMATION ===\n";
                cout << "Railway Ticket Management System\n";
                cout << "Version: 2.0\n";
                cout << "Features included:\n";
                cout << "1. Admin train management\n";
                cout << "2. Passenger ticket booking with unique PNR\n";
                cout << "3. Cheaper route suggestions\n";
                cout << "4. Meal preferences & catering service\n";
                cout << "5. Cancellation probability prediction\n";
                cout << "6. Catering inventory management\n";
                cout << "7. File-based data persistence\n";
                cout << "\nCurrent stats:\n";
                cout << "- Trains: " << trains.size() << endl;
                cout << "- Bookings: " << bookings.size() << endl;
                cout << "- Catering items: " << cateringMenu.size() << endl;
                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            case 4: 
                saveToFile();
                cout << "\n✅ Data saved successfully!\n";
                cout << "Thank you for using Railway Ticket System!\n";
                cout << "Goodbye!\n";
                break;
            default: cout << "Invalid choice! Please enter 1-4.\n";
        }
    } while (mainChoice != 4);

    return 0;
}
 
 
 
 

