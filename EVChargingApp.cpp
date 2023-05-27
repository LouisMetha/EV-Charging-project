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
#include "costs.h"
#include "WeightedGraph.h"
#include "EVCharging.h"

int main() {
	
	srand(time(NULL));
	EVCharging charging;

	charging.printAdjacencyMatrix();
	// charging.BFT(); // task 8.4

	charging.findChargingStations(); //task 3
	charging.findAdjacentWithChargingStations(); // task 4
	charging.findLowestCostAdj(); // task 5
	charging.findNearestChargingStation(); // task 6
	charging.findCheapestStations(); // task 7
	charging.findOnewayCheapest(); // task 8
	charging.findCheapestPathMultipleStops(); // task 9

	return 0;
}
