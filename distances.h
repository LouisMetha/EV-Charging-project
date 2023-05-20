#pragma once

class Distances {
public:
	int index;
	double distance;

	bool operator<(const Distances& d)  const {
		return distance > d.distance;
	}
};