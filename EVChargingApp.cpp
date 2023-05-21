#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <queue>
#include <map>
#include <iomanip>
#include <cfloat>
#include <algorithm>

using namespace std;

#include "Location.h"
#include "distances.h"
#include "WeightedGraph.h"
#include "EVCharging.h"

int main() {
	
	srand(time(NULL));
	EVCharging charging;

	charging.printLocations();
	charging.printAdjacencyMatrix();
	// charging.BFT(); // task 8.4

	charging.findChargingStations(); //task 3
	charging.getCurrentLocation();
	charging.findAdjacentWithChargingStations(); // task 4
	charging.findLowestCost(); // task 5
	charging.findNearestChargingStation(); // task 6

	return 0;
}
