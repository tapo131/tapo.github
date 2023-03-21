#include <iostream>
#include "vehicle.h"
#include "graph.h"
#include "colony.h"
void Car::add_node(Node *a, double** matrix)
{
	if(!tour.empty()){// though the tour first adds the depot, it doesnt count time 
		now_time += (int)matrix[now_idx][a->idx] / car_speed;
		now_time += a->unload_time;	
	}
	tour.push_back(*a);
	now_load += a->demand;
	now_idx = a->idx;
	a->visited = true;
	tour_cost+=matrix[now_idx][a->idx]*car_cost;
}
bool Car::ok_capacity(Node a) const
{
	if(now_load + a.demand <= car_capacity){// put the demand of a node
		return true;
	}
	else{
		return false;
	}
}

bool Car::ok_time(Node a, double** matrix) const
{
	int travel_time = (int)matrix[now_idx][a.idx] / car_speed;
	int arrival_time = now_time + travel_time;
	if(a.tw_open <= arrival_time && arrival_time + a.unload_time <= a.tw_close){
		return true;// arrive after opening && leave before closing
	}
	else{
		return false;
	}
}