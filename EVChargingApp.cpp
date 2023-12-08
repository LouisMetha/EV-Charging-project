#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <queue>
#include <stack>
#include <vector>
#include <map>
#include <iomanip>
#include <cfloat>
#include <algorithm>

using namespace std;

#include "Location.h"
#include "Distances.h"
#include "Costs.h"
#include "WeightedGraph.h"
#include "EVCharging.h"

int main() {
	
	srand(time(NULL));
	EVCharging charging;

	charging.printAdjacencyMatrix();

    int choice;
    do {
        cout << "\nMenu:\n";
        cout << "1. Find charging stations\n";
        cout << "2. Find adjacent locations with charging stations\n";
        cout << "3. Find lowest cost adjacency\n";
        cout << "4. Find nearest charging station\n";
        cout << "5. Find cheapest stations\n";
        cout << "6. Find cheapest single charge\n";
        cout << "7. Find cheapest path with multiple stops\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                charging.BFT();
                break;
            case 2:
                charging.findAdjacentWithChargingStations();
                break;
            case 3:
                charging.findLowestCostAdj();
                break;
            case 4:
                charging.findNearestChargingStation();
                break;
            case 5:
                charging.findCheapestStations();
                break;
            case 6:
                charging.findCheapestSingleCharge();
                break;
            case 7:
                charging.findCheapestPathMultipleStops();
                break;
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    } while (choice != 0);

	return 0;
}