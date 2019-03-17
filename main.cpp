#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

#include <ios>
#include <iostream>
#include <chrono>
#include <string>
#include <utility>
#include <vector>
#include <algorithm>
#include <random>

using namespace std;
using namespace std::chrono;

high_resolution_clock::time_point start;
#define NOW high_resolution_clock::now()
#define TIME duration_cast<duration<double>>(NOW - start).count()

// The watch macro is one of the most useful tricks ever.
#define watch(x) cerr << (#x) << " is " << (x) << "\n"

constexpr int BOARD_WIDTH = 11;
constexpr int BOARD_HEIGHT = 7;
constexpr int ROUND_LENGTH = 200;

// Temps de réponse pour le premier tour ≤ 1s
constexpr double TIME_LIMIT_FIRST_TURN = 0.85;

// Temps de réponse pour un tour de jeu ≤ 50ms
constexpr double TIME_LIMIT = 0.045;

int currentRound = 1;

class Position;

class Cell;

class Chef;

class Customer;

Cell *kitchen[BOARD_WIDTH][BOARD_HEIGHT];
Cell *window;
Cell *dish;
Cell *blueBerries;
Cell *iceCream;
Cell *strawBerries;
Cell *choppingBoard;
Chef *playerChef;
Chef *opponent1Chef;
Chef *opponent2Chef;
vector<Customer *> customers;

int manhattanDistance(int x1, int y1, int x2, int y2) {
    return abs(x2 - x1) + abs(y2 - y1);
}

class Position {
public:
    int x{};
    int y{};
    int sx{};
    int sy{};

    Position() = default;

    Position(int x, int y) {
        this->x = x;
        this->y = y;
        this->sx = x;
        this->sy = y;
    }

    virtual void move(int x, int y) {
        this->x = x;
        this->y = y;
    }

    virtual void reset() {
        this->x = this->sx;
        this->y = this->sy;
    }
};

class Cell : public Position {
public:
    /**
     * .: case de sol
     * #: table de travail
     * D: le lave-vaisselle
     * W: la fenêtre de clients
     * B: la corbeille de myrtilles
     * I: la corbeille de crème glacée
     */
    char type;
    string item;

    Cell(int x, int y, char type) : Position(x, y) {
        this->type = type;
        this->item = "NONE";
    }

    Cell *getNearestOfType(char type) {
        int distance = INFINITY;
        Cell *nearest = nullptr;

        for (int y = 0; y < BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                // Visiting itself doesn't count
                if (this->x == x && this->y == y) {
                    continue;
                }

                if (kitchen[x][y]->type == type && kitchen[x][y]->item == "NONE") {
                    int d = manhattanDistance(this->x, this->y, kitchen[x][y]->x, kitchen[x][y]->y);

                    if (d < distance) {
                        distance = d;
                        nearest = kitchen[x][y];
                    }
                }
            }
        }

        return nearest;
    }

    Cell *getNearestOfItem(string item) {
        int distance = INFINITY;
        Cell *nearest = nullptr;

        for (int y = 0; y < BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                // Visiting itself doesn't count
                if (this->x == x && this->y == y) {
                    continue;
                }

                size_t found = kitchen[x][y]->item.find(item);

                // DISH-BLUEBERRIES MATCH DISH | BLUEBERRIES | DISH-BLUEBERRIES
                if (kitchen[x][y]->item == item || found != string::npos) {
                    int d = manhattanDistance(this->x, this->y, kitchen[x][y]->x, kitchen[x][y]->y);

                    if (d < distance) {
                        distance = d;
                        nearest = kitchen[x][y];
                    }
                }
            }
        }

        return nearest;
    }
};

class Chef : public Position {
public:
    string item;

    Chef() {
        this->item = "NONE";
    }

    Chef(int x, int y, const string &item) : Position(x, y) {
        this->item = item;
    }

    Cell *nextItemToPick(Customer *customer);
};

class Customer {
public:
    string item;
    int award;

    Customer(const string &item, int award) {
        this->item = item;
        this->award = award;
    }
};

Customer *getBestCustomer() {
    int index = 0;
    int award = 0;

    for (int i = 0; i < customers.size(); i++) {
        if (customers[i]->award > award) {
            award = customers[i]->award;
            index = i;
        }
    }

    return customers[index];
}

Cell *Chef::nextItemToPick(Customer *customer) {
    // We deliver to the window if we have all the items needed
    if (this->item == customer->item) {
        return window;
    }

    // If chef doesn't have a dish
    // Check if a dish is already present on any table
    // Or if a dish is present at the dishwasher and is closer, take it
    if (this->item.find("DISH") == -1) {
        auto *nearestDishItem = kitchen[this->x][this->y]->getNearestOfItem("DISH");

        if (nearestDishItem != nullptr) {
            return nearestDishItem;
        } else {
            return dish;
        }
    }

    if (this->item.find("STRAWBERRIES")) {
        return choppingBoard;
    }

    // TODO : BE GENERIC WITH THE ITEM TYPES

    size_t blueBerriesPos = customer->item.find("BLUEBERRIES");
    size_t iceCreamPos = customer->item.find("ICE_CREAM");
    size_t choppedStrawBerriesPos = customer->item.find("CHOPPED_STRAWBERRIES");

    if (blueBerriesPos < iceCreamPos) {
        if (this->item.find("BLUEBERRIES") == -1) {
            auto *nearestBlueBerryItem = kitchen[this->x][this->y]->getNearestOfItem("BLUEBERRIES");

            if (nearestBlueBerryItem != nullptr) {
                return nearestBlueBerryItem;
            } else {
                return blueBerries;
            }
        }

        if (this->item.find("ICE_CREAM") == -1) {
            auto *nearestIceCreamItem = kitchen[this->x][this->y]->getNearestOfItem("ICE_CREAM");

            if (nearestIceCreamItem != nullptr) {
                return nearestIceCreamItem;
            } else {
                return iceCream;
            }
        }
    } else {
        if (this->item.find("ICE_CREAM") == -1) {
            auto *nearestIceCreamItem = kitchen[this->x][this->y]->getNearestOfItem("ICE_CREAM");

            if (nearestIceCreamItem != nullptr) {
                return nearestIceCreamItem;
            } else {
                return iceCream;
            }
        }

        if (this->item.find("BLUEBERRIES") == -1) {
            auto *nearestBlueBerryItem = kitchen[this->x][this->y]->getNearestOfItem("BLUEBERRIES");

            if (nearestBlueBerryItem != nullptr) {
                return nearestBlueBerryItem;
            } else {
                return blueBerries;
            }
        }
    }

    return dish;
}

int main() {
    int numAllCustomers;
    cin >> numAllCustomers;
    cin.ignore();

    for (int i = 0; i < numAllCustomers; i++) {
        // the food the customer is waiting for
        string customerItem;

        // the number of points awarded for delivering the food
        int customerAward;

        cin >> customerItem >> customerAward;
        cin.ignore();
        auto *customer = new Customer(customerItem, customerAward);
        customers.emplace_back(customer);
    }

    for (int i = 0; i < BOARD_HEIGHT; i++) {
        string kitchenLine;
        getline(cin, kitchenLine);
//        cerr << kitchenLine << endl;

        for (int j = 0; j < BOARD_WIDTH; j++) {
            kitchen[j][i] = new Cell(j, i, kitchenLine[j]);

            if (kitchen[j][i]->type == 'W') {
                window = kitchen[j][i];
            } else if (kitchen[j][i]->type == 'D') {
                dish = kitchen[j][i];
            } else if (kitchen[j][i]->type == 'B') {
                blueBerries = kitchen[j][i];
            } else if (kitchen[j][i]->type == 'I') {
                iceCream = kitchen[j][i];
            } else if (kitchen[j][i]->type == 'S') {
                strawBerries = kitchen[j][i];
            } else if (kitchen[j][i]->type == 'C') {
                choppingBoard = kitchen[j][i];
            }
        }
    }

//    cerr << endl;
//    for (int y = 0; y < BOARD_HEIGHT; y++) {
//        for (int x = 0; x < BOARD_WIDTH; x++) {
//            cerr << kitchen[x][y]->type;
//        }
//
//        cerr << endl;
//    }

    playerChef = new Chef();
    opponent1Chef = new Chef();
    opponent2Chef = new Chef();

    // game loop
    while (true) {
        int turnsRemaining;
        cin >> turnsRemaining;
        cin.ignore();
        start = NOW;

        int playerX;
        int playerY;
        string playerItem;
        cin >> playerX >> playerY >> playerItem;
        cin.ignore();
        playerChef->x = playerX;
        playerChef->y = playerY;
        playerChef->sx = playerX;
        playerChef->sy = playerY;
        playerChef->item = playerItem;

        int partnerX;
        int partnerY;
        string partnerItem;
        cin >> partnerX >> partnerY >> partnerItem;
        cin.ignore();

        if (currentRound == 1) {
            opponent1Chef->x = partnerX;
            opponent1Chef->y = partnerY;
            opponent1Chef->sx = partnerX;
            opponent1Chef->sy = partnerY;
            opponent1Chef->item = partnerItem;
        } else {
            opponent2Chef->x = partnerX;
            opponent2Chef->y = partnerY;
            opponent2Chef->sx = partnerX;
            opponent2Chef->sy = partnerY;
            opponent2Chef->item = partnerItem;
        }

        // the number of tables in the kitchen that currently hold an item
        int numTablesWithItems;
        cin >> numTablesWithItems;
        cin.ignore();

        for (int i = 0; i < numTablesWithItems; i++) {
            int tableX;
            int tableY;
            string item;
            cin >> tableX >> tableY >> item;
            cin.ignore();
            kitchen[tableX][tableY]->item = item;
        }

        // ignore until wood 1 league
        string ovenContents;
        int ovenTimer;
        cin >> ovenContents >> ovenTimer;
        cin.ignore();

        // the number of customers currently waiting for food
        int numCustomers;
        cin >> numCustomers;
        cin.ignore();

        for (int i = 0; i < numCustomers; i++) {
            string customerItem;
            int customerAward;
            cin >> customerItem >> customerAward;
            cin.ignore();
            customers[i]->item = customerItem;
            customers[i]->award = customerAward;
        }

        // *************************************************************************************************************
        // GAME LOGIC
        // *************************************************************************************************************

        auto *bestCustomer = getBestCustomer();
        auto *nextCell = playerChef->nextItemToPick(bestCustomer);

        cout << "USE " << nextCell->x << " " << nextCell->y << endl;

        if (turnsRemaining == 0) {
            currentRound++;
        }
    }
}

#pragma clang diagnostic pop