#ifndef EVCHARGING_H_
#define EVCHARGING_H_

class EVCharging {
private:
	map<int, Location> locations; //can also be unordered_map
	int numberOfLocations;
	WeightedGraphType* weightedGraph;
	priority_queue<Location> chargingStations;
	list<int> adjacentLocations;
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
	void findCheapestSingleCharge();
	int getLocation();
	void findCheapestPathMultipleStops();
	int cheapestMidPointStation(int origin, int dest, int dest_charge_amount, double& origin_cost);
	int cheapestMultiStops(int origin, int dest, int dest_charge_amount, double& cost);
	void printPath(stack<int> path);

	int getKeyFromName(const string& name) {
		for (const auto& pair : locations){
			if (pair.second.locationName == name)
				return pair.first;
		}
		return -1;
	}
};

EVCharging::EVCharging() {
	inputLocations();
	weightedGraph = new WeightedGraphType(numberOfLocations);
	printLocations();
}

EVCharging::~EVCharging() {
	delete weightedGraph;
}

void EVCharging::inputLocations() {
	ifstream infile;
	char fileName[50] = "Locations.txt";

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

int EVCharging::getLocation() {

    string name;
    bool locationFound = false;
	int key;
	cin.ignore();

    while (!locationFound) {
        getline(cin,name);
        key = getKeyFromName(name);
        if (key != -1) {
            locationFound = true;
        } else {
            cout << "Location not found. Please enter a valid location: ";
        }
    }

	return key;
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

// Need to upodate find 5 closest charging stations based on user's current location/coordinate
void EVCharging::findAdjacentWithChargingStations() {
	
	cout << "Enter current location: ";
	int key_location = getLocation();
	adjacentLocations = weightedGraph->getAdjancencyList(key_location);

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

// Retrieve cost from API then calculate costs, use  PQ for min-cost.
void EVCharging::findLowestCostAdj() {
	
	charge_amount = rand() % 41 + 10;
	cout << "Enter current location: ";
	int key_location = getLocation();
	adjacentLocations = weightedGraph->getAdjancencyList(key_location);

	priority_queue<Costs> lowest;
	bool none = false;
	cout << setw (20) << "Location name" << setw(20) << "Required amount" << setw(20) <<"Charging amount" 
	<< setw(12) << "Distance" << setw(20) << "Charging price" << setw(12) << "Cost" << endl;

	for (int l : adjacentLocations) {
		if (locations[l].chargerInstalled) {
			Costs c;
			c.index = l;
			c.cost = calculateCost(locations[l].chargingPrice, weightedGraph->getWeight(key_location, l), locations[l].locationName);
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

	charge_amount = rand() % 41 + 10;
	cout << "Enter current location: ";
	int key_location = getLocation();
	distances = weightedGraph->shortestPath(key_location);

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

	cout << "Enter current location: ";
	charge_amount = rand() % 41 + 10;
	int key_location = getLocation();
	distances = weightedGraph->shortestPath(key_location);
	priority_queue<Costs> sortedCost;

	cout << setw (20) << "Location name" << setw(20) << "Required amount" << setw(20) <<"Charging amount" 
	<< setw(12) << "Distance" << setw(20) << "Charging price" << setw(12) << "Cost" << endl;

	for (int i = 0; i < weightedGraph->getSize(); i++) {
		if (locations[i].chargerInstalled && i != key_location) {
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

void EVCharging::findCheapestSingleCharge() {
	
	charge_amount = rand() % 41 + 10;
	cout << "Enter current location: ";
	int current_location = getLocation();
	cout << "Enter destination location: ";
	int dest_location = getLocation();
	cout << endl;

	int origin = current_location;	
	int destination = dest_location;
	int charge_location = 0;
	double cost = 0;

	charge_location = cheapestMidPointStation(origin, destination, charge_amount, cost);
	
	cout << "From " << locations[origin].locationName << " to " << locations[destination].locationName << endl;
	cout << "Charge amount: " << charge_amount << "kWh" << endl;
	cout << "Cheapest to charge at: " << locations[charge_location].locationName << endl;
	cout << "Path: ";

	if (charge_location != origin){
		stack<int> path1 = weightedGraph->shortestPath(origin, charge_location);
		if(path1.empty())
			cout << locations[origin].locationName << " > ";
		printPath(path1);
	}

	stack<int> path2 = weightedGraph->shortestPath(charge_location, destination);
	if(path2.empty()) {
		if (dest_location != charge_location) {
			cout << locations[charge_location].locationName << " > ";
		}
	}
	printPath(path2);

	cout << locations[destination].locationName << endl;
	cout << "Total cost: $" << cost << endl;
}

void EVCharging::findCheapestPathMultipleStops() {

	charge_amount = rand() % 41 + 10;
	cout << "Enter current location: ";
	int current_location = getLocation();
	cout << "Enter destination location: ";
	int dest_location = getLocation();
	cout << endl;

	int origin = current_location;
	int destination = dest_location;
	int charge_location = 0;
	double cost = 0;
	int counter = 0;

	while (destination > weightedGraph->getSize()) {
		cout << "\nEnter destiantion number: ";
		cin >> destination;
	}
	cout << "Location: " << locations[destination].locationName << endl;

	if (charge_amount <= 25) {

		charge_location = cheapestMidPointStation(origin, destination, charge_amount, cost);
		
		cout << "From " << locations[origin].locationName << " to " << locations[destination].locationName << endl;
		cout << "Charge amount: " << charge_amount << "kWh" << endl;
		cout << "Cheapest to charge at: " << locations[charge_location].locationName << endl;
		cout << "Path: ";

		if (charge_location != origin){
			stack<int> path1 = weightedGraph->shortestPath(origin, charge_location);
			if(path1.empty())
				cout << locations[origin].locationName << " > ";
			printPath(path1);
		}

		if (charge_location != destination) {
			stack<int> path2 = weightedGraph->shortestPath(charge_location, destination);
			if(path2.empty())
				cout << locations[charge_location].locationName << " > ";
			printPath(path2);
		}

		cout << locations[destination].locationName << endl;
		cout << "Total cost: $" << cost << endl;

	} else {
		
		double prevCost = cost;
		cout << "Total Charge amount: " << charge_amount << "kWh" << endl << endl;
		queue<int> prevLocation;
		int last_push = -1;

		while (charge_amount > 0) {
			if(counter == 0 ){
				prevLocation.push(origin);
				last_push = origin;
			}
			counter++;
			prevCost = cost;
			charge_location = cheapestMultiStops(origin, destination, charge_amount, cost);

			stack<int> path = weightedGraph->shortestPath(origin, charge_location);

			while(!path.empty()) {
				if (last_push == path.top()) {
					path.pop();
				}

				prevLocation.push(path.top());
				path.pop();
			}

			if (charge_amount > 25) {
				charge_amount -= 25;
				if(charge_location != origin) {
					prevLocation.push(charge_location);
					last_push = charge_location;
				}
			} else {
				charge_amount -= charge_amount;
			}
			
			origin = charge_location;

			cout << "Charge at: " << locations[charge_location].locationName << " for $" << cost - prevCost << endl;
			cout << "Remaining charge amount: " << charge_amount << endl << endl;
		}

		prevCost = cost;
		charge_location = cheapestMidPointStation(origin, destination, charge_amount, cost);
		
		cout << "Path: ";
		while (!prevLocation.empty()) {
			cout << locations[prevLocation.front()].locationName << " > ";
			prevLocation.pop();
		}
		
		if (charge_location != origin){
			stack<int> path1 = weightedGraph->shortestPath(origin, charge_location);
			if(path1.empty())
				cout << locations[origin].locationName << " > ";
			printPath(path1);
		}

		if (charge_location != destination) {
			stack<int> path2 = weightedGraph->shortestPath(charge_location, destination);
			if(path2.empty())
				cout << locations[charge_location].locationName << " > ";
			printPath(path2);
		}

		cout << locations[destination].locationName << endl;
		cout << "Remaining travel cost: $" << cost << endl;
		cout << "Total cost: $" <<  prevCost + cost << endl;
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

		} else if (dest_charge_amount > 25  && locations[i].chargingPrice > 0) {
			Costs c;
			c.index = i;
			c.cost = (distances_origin[i] + distances_dest[i]) * travel_cost + dest_charge_amount * locations[i].chargingPrice;
			sortedCost.push(c);
		}
	}

	cost = sortedCost.top().cost;
	return sortedCost.top().index;
}

void EVCharging::printPath(stack<int> path) {
	while(!path.empty()) {
		cout << locations[path.top()].locationName << " > ";
		path.pop();
	}
}

int EVCharging::cheapestMultiStops(int origin, int dest, int dest_charge_amount, double& cost) {

	double* distances_origin = weightedGraph->shortestPath(origin);
	double* distances_dest = weightedGraph->shortestPath(dest);
	double travel_cost = 0.1;

	priority_queue<Costs> sortedCost;

	for (int i = 0; i < weightedGraph->getSize(); i++) {
		if (dest_charge_amount <= 25 && locations[i].chargerInstalled && locations[i].index != origin) {
			Costs c;
			c.index = i;
			c.cost = (distances_origin[i] + distances_dest[i]) * travel_cost + dest_charge_amount * locations[i].chargingPrice;
			sortedCost.push(c);

		} else if (dest_charge_amount > 25 && locations[i].index == origin && locations[i].chargingPrice == 0) {
			Costs c;
			c.index = i;
			c.cost = (distances_origin[i] + distances_dest[i]) * travel_cost + dest_charge_amount * locations[i].chargingPrice;
			sortedCost.push(c);

		} else if (locations[i].index == origin && locations[i].chargingPrice == 0) {
			continue;

		} else if (locations[i].chargerInstalled) {
			Costs c;
			c.index = i;
			c.cost = (distances_origin[i] + distances_dest[i]) * travel_cost + dest_charge_amount * locations[i].chargingPrice;
			sortedCost.push(c);
		}
	}

	if (dest_charge_amount > 0) {
		cost = cost + (distances_origin[sortedCost.top().index] * travel_cost + dest_charge_amount * locations[sortedCost.top().index].chargingPrice);
	} else {
		cost = cost + sortedCost.top().cost;
	}

	return sortedCost.top().index;
}

#endif /* EVCHARGING_H_ */
