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

class Position;

class Cell;

class Chef;

class Recipe;

class Customer;

Cell *kitchen[BOARD_WIDTH][BOARD_HEIGHT];
Chef *playerChef;
Chef *opponent1Chef;
Chef *opponent2Chef;
Customer *customers[];

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

class Cell : Position {
public:
    char type = '.';

    Cell(int x, int y, char type) : Position(x, y) {
        this->type = type;
    }
};

class Customer {
public:
    string food;
    int award;

    Customer(const string &food, int award) {
        this->food = food;
        this->award = award;
    }
};

int main() {
    // *****************************************************************************************************************
    // <FIRST TURN>
    // *****************************************************************************************************************

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
        customers[i] = new Customer(customerItem, customerAward);
    }

    for (int i = 0; i < BOARD_HEIGHT; i++) {
        string kitchenLine;
        getline(cin, kitchenLine);
        cerr << kitchenLine << endl;

        for (int j = 0; j < BOARD_WIDTH; j++) {
            kitchen[i][j] = new Cell(i, j, kitchenLine[j]);
        }
    }

    // *****************************************************************************************************************
    // </FIRST TURN>
    // *****************************************************************************************************************

    // game loop
    while (true) {
        int turnsRemaining;
        cin >> turnsRemaining;
        cin.ignore();

        int playerX;
        int playerY;
        string playerItem;
        cin >> playerX >> playerY >> playerItem;
        cin.ignore();

        int partnerX;
        int partnerY;
        string partnerItem;
        cin >> partnerX >> partnerY >> partnerItem;
        cin.ignore();

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
        }

        // MOVE x y
        // USE x y
        // WAIT
        cout << "WAIT" << endl;
    }
}

#pragma clang diagnostic pop