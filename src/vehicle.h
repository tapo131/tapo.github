#include <vector>
#ifndef VEHICLE_H
#define VEHICLE_H

class Node{
public:
	double x, y;
	int idx, demand;
	bool visited;
	int tw_open, tw_close, unload_time;//time windows
	int service_time;
	double penalty_cost;
};
class Car{
public:
	int car_capacity, now_load, now_idx;
	int car_speed, now_time;//time windows
	double car_cost, tour_cost;
	std::vector <Node> tour; 
	void add_node(Node*, double**);// constraint with car
	bool ok_capacity(Node) const;
	bool ok_time(Node, double**) const;//time windows
};
#endif