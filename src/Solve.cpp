#include <iostream>
#include <vector>
#include <random> // rand_dev
#include <algorithm> // std::max_element
#include "colony.h"
#include "Graph.h"
#include "vehicle.h"
#include <time.h>	
void Colony::run_ACO()
{
	double min_distance = 100000;
	for(int i = 0; i < ant_time_step; i++){
		std::cout << std::endl << "time step: " << i << std::endl;
		move_ants();// start moving colony
		double total_tour_distance = 0.0;
		//std::cout<<total_tour_distance<<" ";
		update_phero_matrix(total_tour_distance); //cap nhat mui
		if (total_tour_distance < min_distance){
			min_distance = total_tour_distance;	
			show_each_car_tour();	//print tours
		}		
		update_threshold(); // update
		reset_at_each_step();
	}
}
void Colony::reset_at_each_step() // reset 
{
	for (int i = 0; i < num_car; i++){
		cars[i].tour.clear();
		cars[i].now_load = 0;
		cars[i].now_idx = 0;
		cars[i].now_time = 0;
	}
	for (int i = 0; i < num_node; i++){
		nodes[i].visited = false;
	}
}
