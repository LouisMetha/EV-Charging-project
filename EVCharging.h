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
	int currentLocation = INT_MAX;

public:
	EVCharging();
	~EVCharging();
	void inputLocations();
	void printLocations();
	void printAdjacencyMatrix();
	void findChargingStations();
	void BFT();
	void printChargingStations(const priority_queue<Location>& l);
	void findLowestCost();
	void findAdjacentWithChargingStations();
	double calculateCost(double price, int weight, int charge_amount);
	void findNearestChargingStation();
	void getCurrentLocation();
};

EVCharging::EVCharging() {
	inputLocations();
	weightedGraph = new WeightedGraphType(numberOfLocations);
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
	
	list<int> adjacentLocations;
	bool none = false;

	adjacentLocations = weightedGraph->getAdjancencyList(currentLocation);
	
	cout << "\nList of adjacent locations with charging stations ----------------\n\n";
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

void EVCharging::findLowestCost() {

	list<int> adjacentLocations;
	priority_queue<Location> lowest;
	bool none = false;
	int charge_amount = rand() % 41 + 10;

	adjacentLocations = weightedGraph->getAdjancencyList(currentLocation);

	for (int l : adjacentLocations) {
		if (locations[l].chargerInstalled) {
			lowest.push(locations[l]);
			none = true;
		}
	}
	
	if (!none) {
		cout << "---NONE---";
	} else {
		cout << "\nAdjacent location with the lowest charging cost: " << lowest.top().locationName << endl;
		double cost = calculateCost(lowest.top().chargingPrice, weightedGraph->getWeight(currentLocation,lowest.top().index),charge_amount);
		cout << "Cost: $" << cost << endl;
	}
}

double EVCharging::calculateCost(double price, int weight ,int charge_amount) {

	double travel_cost = 0.1;
	cout << "Required charging amount: " << charge_amount << "kWh" << endl;

	if (price == 0 && charge_amount > 25)
		charge_amount = 25;

	cout << "Charging amount: " << charge_amount << "kWh" << endl;
	cout << "Distance: " << weight << "km" << endl;
	cout << "Charging price: $" << price << endl;

	return (weight * 2 * travel_cost) + (charge_amount * price);
}

void EVCharging::findNearestChargingStation() {

	priority_queue<Distances> sortedDistances;
	double* distances = weightedGraph->shortestPath(currentLocation);

	for (int i = 0; i < weightedGraph->getSize(); i++) {
		Distances d;
		d.index = i;
		d.distance = distances[i];
		sortedDistances.push(d);
	}

	while (!sortedDistances.empty()) {
		if (!sortedDistances.top().distance == 0 && locations[sortedDistances.top().index].chargerInstalled) {
			cout << "\nThe nearest charging station: " << locations[sortedDistances.top().index].locationName << endl;
			cout << "Disctance: " << int(sortedDistances.top().distance) << "km.";
			break;
		}
		sortedDistances.pop();
	}
}

void EVCharging::getCurrentLocation() {

	while (currentLocation > weightedGraph->getSize()){
		cout << "\nEnter the location number: ";
		cin >> currentLocation;
	}

	cout << "Location: " << locations[currentLocation].locationName << endl;
}

#endif /* EVCHARGING_H_ */
