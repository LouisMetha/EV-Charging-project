class Costs {
public:
	int index;
	double cost;
	double distance;

	bool operator<(const Costs& c)  const {
		return cost > c.cost;
	}
};