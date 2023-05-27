/*
 * EVCharging.h
 *
 *  Created on: 29 Apr 2023
 *      Author: 2002345
 */

#ifndef EVCHARGING_H_
#define EVCHARGING_H_

class EVCharging {// you may also declare this class as an extension of WeightedGraph
private:
	map<int, Location> locations;//It can also be unordered_map
	int numberOfLocations;
	WeightedGraphType* weightedGraph;
	priority_queue<Location> chargingStations;
	list<int> adjacentLocations;
	int currentLocation = INT_MAX;
	int charge_amount = rand() % 41 + 10;
	double* distances;

public:
	EVCharging();
	~EVCharging();
	void inputLocations();
	void printLocations();
	void printAdjacencyMatrix();
	void findChargingStations();
	void BFT();
	void printChargingStations(const priority_queue<Location>& l);
	void findLowestCostAdj();
	void findAdjacentWithChargingStations();
	double calculateCost (double price, int weight, string lname) const;
	double calculateCostOneway(double price, int weight, string lname) const;
	void findNearestChargingStation();
	void findCheapestStations();
	void findOnewayCheapest();
	void getCurrentLocation();
	void findCheapestPathMultipleStops();
	int findCheapestMidPoint(int origin, int dest, int dest_charge_amount, double& cost);
	int cheapestMidPointStation(int origin, int dest, int dest_charge_amount, double& origin_cost);
	int cheapestMultiStops(int origin, int dest, int dest_charge_amount, double& cost);
};

EVCharging::EVCharging() {
	inputLocations();
	weightedGraph = new WeightedGraphType(numberOfLocations);
	printLocations();
	getCurrentLocation();
	adjacentLocations = weightedGraph->getAdjancencyList(currentLocation);
	distances = weightedGraph->shortestPath(currentLocation);

}

EVCharging::~EVCharging() {
	delete weightedGraph;
}

void EVCharging::inputLocations() {
	ifstream infile;
	char fileName[50] = "Locations.txt";

//	cout << "Enter charging location file name: ";
//	cin >> fileName;
//	cout << endl;

	infile.open(fileName);

	if (!infile) {
		cout << "Cannot open input file." << endl;
		return;
	}

	int locationIndex = 0;

	while (!infile.eof()) {
		Location s;
		string charger;
		string price;
		while (!infile.eof()) {
			getline(infile, s.locationName, ',');
			getline(infile, charger, ',');
			getline(infile, price);
			s.chargerInstalled = (stoi(charger) == 1) ? true : false;
			s.chargingPrice = stod(price);
			s.index = locationIndex;
			locations[locationIndex] = s;
			locationIndex++;
		}
	}

	numberOfLocations = locationIndex;
}

void EVCharging::getCurrentLocation() {

	while (currentLocation > weightedGraph->getSize()){
		cout << "\nEnter current location number: ";
		cin >> currentLocation;
	}

	cout << "Location: " << locations[currentLocation].locationName << endl;
}

void EVCharging::printLocations() {
	cout << "List of locations and charging information " << endl;
	cout << setw (8) << "Index" << setw (20) << "Location name" << setw (20) <<"Charging station" << setw(20) << "Charging price" << endl;

	map<int, Location>::iterator it = locations.begin();

	for (; it != locations.end(); it++) {
		it->second.printLocation();
	}

    cout << endl;
}

void EVCharging::printAdjacencyMatrix() {
	cout << "Adjacency matrix (0 means no direct connection, non-zero value represents the distance of adjacent locations)\n" << endl;
	cout << setw(13) << " ";
	for (int i = 0; i < numberOfLocations; i++) {
		cout << setw(13) << locations[i].locationName;
	}
	cout << endl;
	for (int i = 0; i < numberOfLocations; i++) {
		cout << setw(13) << locations[i].locationName;
		for (int j = 0; j < numberOfLocations; j++) {
			cout << setw(13) << (weightedGraph->getWeight(i,j) == DBL_MAX ? 0.0 : weightedGraph->getWeight(i,j));
		}
		cout << endl;
	}
}

// Task 8.4 ************************************************************************
void EVCharging::BFT() {

	list<int> indices = weightedGraph->breadthFirstTraversal();

	cout << "\nResult of BFT: ";
	for (int index : indices) {
		cout << " " << index << " ";
	}
}

void EVCharging::findChargingStations() {

	cout << "\n====== Task 3 - list all charging stations in ascending order of charging price ======\n\n";

	for (auto l : locations) {
		if (l.second.chargerInstalled) {
			chargingStations.push(l.second);
		}
	}

	printChargingStations(chargingStations);
}

void EVCharging::printChargingStations(const priority_queue<Location>& l) {

	priority_queue<Location> temp = l;
	cout << "\nlist of locations with charging stations ---------------------------- \n\n";
	cout << setw (8) << "Index" << setw (20) << "Location name" << setw (20) <<"Charging station" << setw(20) << "Charging price" << endl;

	while (!temp.empty()) {
		Location s = temp.top();
		s.printLocation();
		temp.pop();
	}
}

void EVCharging::findAdjacentWithChargingStations() {
	
	cout << "\n====== Task 4 - List adjacent charging stations ======\n\n";

	bool none = false;
	cout << "\nList of adjacent locations with charging stations ----------------\n";
	cout << setw (8) << "Index" << setw (20) << "Location name" << setw (20) <<"Charging station" << setw(20) << "Charging price" << endl;

	for (int a : adjacentLocations) {
		if (locations[a].chargerInstalled) {
			locations[a].printLocation();
			none = true;
		}
	}
	
	if (!none)
		cout << "---NONE---";
}

void EVCharging::findLowestCostAdj() {

	cout << "\n====== Task 5 - Find the adjacent charging station with lowest travel & charging cost ======\n\n";

	priority_queue<Costs> lowest;
	bool none = false;
	cout << setw (20) << "Location name" << setw(20) << "Required amount" << setw(20) <<"Charging amount" 
	<< setw(12) << "Distance" << setw(20) << "Charging price" << setw(12) << "Cost" << endl;

	for (int l : adjacentLocations) {
		if (locations[l].chargerInstalled) {
			Costs c;
			c.index = l;
			c.cost = calculateCost(locations[l].chargingPrice, weightedGraph->getWeight(currentLocation, l), locations[l].locationName);
			lowest.push(c);
			none = true;
		}
	}
	
	while (none && charge_amount > 25 && locations[lowest.top().index].chargingPrice == 0) {
		if (!lowest.empty()) {
			cout << "\n**Unable to charge at free station (>25kWh): " << locations[lowest.top().index].locationName;
			lowest.pop();
		} else {
			break;
		}
	}

	cout << "\nAdjacent location with the lowest charging cost: ";
	if (!none) {
		cout << "---NONE---";
	} else {
		cout << locations[lowest.top().index].locationName << endl;
		cout << "Charge amount: " << charge_amount << "kWh" << endl;
		cout << "Charging cost: $" << lowest.top().cost << endl;
	}
}

double EVCharging::calculateCostOneway(double price, int weight, string lname) const {
	
	cout << setw(18) << lname;
	cout << setw(15) << charge_amount << "kWh";
	
	int limit_amount = 25;
	double cost;
	double travel_cost = 0.1;

	if (price == 0 && charge_amount > 25){
		cost = (weight * travel_cost) + (limit_amount * price);
		cout << setw(17) << limit_amount << "kWh";

	} else {
		cost = (weight * travel_cost) + (charge_amount * price);
		cout << setw(17) << charge_amount << "kWh";
	}

	cout << setw(12) << int(weight) << "km";
	cout << setw(12) << "$" << fixed << setprecision(2) << price << "/kWn";
	cout << setw(10) << "$" << cost << endl;

	return cost;
}

double EVCharging::calculateCost(double price, int weight, string lname) const {
	
	cout << setw(18) << lname;
	cout << setw(15) << charge_amount << "kWh";
	
	double cost;
	double travel_cost = 0.1;

	if (price == 0 && charge_amount > 25) {
		cout << setw(17) << charge_amount << "kWh";
		cout << setw(12) << int(weight) << "km";
		cout << setw(12) << "$" << fixed << setprecision(2) << price << "/kWn";
		cout << setw(14) << "N/A" << endl;
		return 0;
	}

	cost = (weight * 2 * travel_cost) + (charge_amount * price);

	cout << setw(17) << charge_amount << "kWh";
	cout << setw(12) << int(weight) << "km";
	cout << setw(12) << "$" << fixed << setprecision(2) << price << "/kWn";
	cout << setw(10) << "$" << cost << endl;

	return cost;
}

void EVCharging::findNearestChargingStation() {

	cout << "\n====== Task 6 - Find the nearest charging station ======\n\n";

	priority_queue<Distances> sortedDistances;

	for (int i = 0; i < weightedGraph->getSize(); i++) {
		Distances d;
		d.index = i;
		d.distance = distances[i];
		sortedDistances.push(d);
	}

	while (!sortedDistances.empty()) {
		if (!sortedDistances.top().distance == 0 && locations[sortedDistances.top().index].chargerInstalled) {
			cout << "\nThe nearest charging station: " << locations[sortedDistances.top().index].locationName << endl;
			cout << "Distance: " << sortedDistances.top().distance << "km\n\n";
			break;
		}
		sortedDistances.pop();
	}
}

void EVCharging::findCheapestStations() {

	cout << "\n====== Task 7 - Find the cheapest charging station other than the current location ( >25kWh cannot charge at free station*) ======\n\n";
	
	priority_queue<Costs> sortedCost;
	cout << setw (20) << "Location name" << setw(20) << "Required amount" << setw(20) <<"Charging amount" 
	<< setw(12) << "Distance" << setw(20) << "Charging price" << setw(12) << "Cost" << endl;

	for (int i = 0; i < weightedGraph->getSize(); i++) {
		if (locations[i].chargerInstalled && i != currentLocation) {
			Costs c;
			c.index = i;
			c.distance = distances[i];
			c.cost = calculateCost(locations[i].chargingPrice, distances[i],locations[i].locationName);
			sortedCost.push(c);
		}
	}

	while (!sortedCost.empty()) {
		if (sortedCost.top().cost != 0) {
			cout << "Charge amount: " << charge_amount << "kWh" << endl;
			cout << "Cheapest charging station: " << locations[sortedCost.top().index].locationName << endl;
			cout <<  "Total cost: $" << sortedCost.top().cost << endl << endl;
			break;
		}
		sortedCost.pop();
	}
}

void EVCharging::findOnewayCheapest() {

	cout << "\n====== Task 8 - Find the cheapest charging station between origin and destination (single charging) ======\n\n";

	int origin = currentLocation;
	int destination = INT_MAX;
	int charge_location = 0;
	double cost = 0;

	while (destination > weightedGraph->getSize()){
		cout << "\nEnter destiantion number: ";
		cin >> destination;
	}
	cout << "Location: " << locations[destination].locationName << endl << endl;

	charge_location = cheapestMidPointStation(origin, destination, charge_amount, cost);

	cout << "From " << locations[origin].locationName << " to " << locations[destination].locationName << endl;
	cout << "Charge amount: " << charge_amount << "kWh" << endl;
	cout << "Cheapest charging station: " << locations[charge_location].locationName << endl;
	cout << "Total cost: $" << cost << endl;
}

void EVCharging::findCheapestPathMultipleStops() {

	int destination = INT_MAX;
	int origin = currentLocation;
	double cost = 0;
	int counter = 0;

	while (destination > weightedGraph->getSize()){
		cout << "\nEnter destiantion number: ";
		cin >> destination;
	}
	cout << "Location: " << locations[destination].locationName << endl;

	if (charge_amount <= 25) {

		int charge_location = findCheapestMidPoint(origin, destination, charge_amount, cost);
		counter++;
		cout << "Charge at: " << locations[charge_location].locationName << endl;
		cout << "Remaining charge amount: " << charge_amount << endl; 
		cout << "Cheapest oneway from " << locations[currentLocation].locationName << " to " << locations[destination].locationName << " is $" << cost << endl;
		cout << "Number of charge(s): " << counter << " at " << locations[charge_location].locationName << endl;

	} else {
		
		double prevCost = cost;
		int charge_location;

		while (charge_amount != 0 || charge_location != destination) {
			counter++;
			prevCost = cost;
			charge_location = findCheapestMidPoint(origin, destination, charge_amount, cost);
			if (charge_amount > 25) {
				charge_amount -= 25;
			} else {
				charge_amount -= charge_amount;
			}
			origin = charge_location;
			cout << "Charge at: " << locations[charge_location].locationName << " for $" << cost - prevCost << endl;
			cout << "Remaining charge amount: " << charge_amount << endl; 
		}

		cout << "\nCheapest oneway from " << locations[currentLocation].locationName << " to " << locations[destination].locationName << " is $" << cost << endl;
		cout << "Number of charge(s): " << counter << endl;
	}
}

int EVCharging::cheapestMidPointStation(int origin, int dest, int dest_charge_amount, double& cost) {

	double* distances_origin = weightedGraph->shortestPath(origin);
	double* distances_dest = weightedGraph->shortestPath(dest);
	double travel_cost = 0.1;

	priority_queue<Costs> sortedCost;

	for (int i = 0; i < weightedGraph->getSize(); i++) {
		if (dest_charge_amount <= 25 && locations[i].chargerInstalled) {
			Costs c;
			c.index = i;
			c.cost = (distances_origin[i] + distances_dest[i]) * travel_cost + dest_charge_amount * locations[i].chargingPrice;
			sortedCost.push(c);
			cout << "cost at " << locations[i].locationName << ": " << c.cost << endl;

		} else if ( dest_charge_amount > 25  && locations[i].chargingPrice > 0) {
			Costs c;
			c.index = i;
			c.cost = (distances_origin[i] + distances_dest[i]) * travel_cost + dest_charge_amount * locations[i].chargingPrice;
			sortedCost.push(c);
			cout << "cost at " << locations[i].locationName << ": " << c.cost << endl;
		}
	}
	
	cout << "\nDis from " << locations[origin].locationName << " to " << locations[sortedCost.top().index].locationName << ": " << distances_origin[sortedCost.top().index] << endl;
	cout << "Dis from " << locations[sortedCost.top().index].locationName << " to " << locations[dest].locationName << ": " << distances_dest[sortedCost.top().index] << endl;

	cost = sortedCost.top().cost;

	return sortedCost.top().index;
}

int EVCharging::cheapestMultiStops(int origin, int dest, int dest_charge_amount, double& cost) {




}

int EVCharging::findCheapestMidPoint(int origin, int dest, int dest_charge_amount, double& cost) {

	double* distances_origin = weightedGraph->shortestPath(origin);
	double* distances_dest = weightedGraph->shortestPath(dest);
	double travel_cost = 0.1;
	double origin_cost;
	double dest_cost;

	priority_queue<Costs> sortedCost;

	for (int i = 0; i < weightedGraph->getSize(); i++) {

		Costs c;

		if (locations[i].chargingPrice > 0 && dest_charge_amount > 25) {

			c.index = i;
			c.distance = distances_origin[i];
			origin_cost = (distances_origin[i] * travel_cost) + (dest_charge_amount * locations[i].chargingPrice);
			dest_cost = (distances_dest[i] * travel_cost);
			c.cost = origin_cost + dest_cost;

		} else if (locations[i].chargingPrice == 0 && dest_charge_amount > 25) {

			c.index = i;
			c.distance = distances_origin[i];
			origin_cost = (distances_origin[i] * travel_cost) + (25 * locations[i].chargingPrice);
			dest_cost = (distances_dest[i] * travel_cost) + ((dest_charge_amount - 25) * locations[dest].chargingPrice);
			c.cost = origin_cost + dest_cost;

		} else if (locations[i].chargerInstalled && dest_charge_amount <= 25) {

			c.index = i;
			c.distance = distances_origin[i];
			origin_cost = (distances_origin[i] * travel_cost) + (dest_charge_amount * locations[i].chargingPrice);
			dest_cost = (distances_dest[i] * travel_cost);

		} else if (!locations[i].chargerInstalled && charge_amount == 0 && locations[i].index == dest) {

			c.index = i;
			c.distance = distances_origin[i];
			origin_cost = (distances_origin[i] * travel_cost);
			dest_cost = (distances_dest[i] * travel_cost);

		} else {

			c.index = i;
			c.distance = distances_origin[i];
			origin_cost = DBL_MAX;
			dest_cost = 0;
		}

		c.cost = origin_cost + dest_cost;
		sortedCost.push(c);	
	}
	
	// cout << "top up Cost: " << sortedCost.top().cost << " from " << locations[origin].locationName << " to " << locations[sortedCost.top().index].locationName << endl; 
	cost = cost + sortedCost.top().cost;
	return sortedCost.top().index;
}

#endif /* EVCHARGING_H_ */
