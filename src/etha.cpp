#include <iostream>
#include <vector>
#include <random> // rand_dev
#include <algorithm> // std::max_element
#include "colony.h"
#include "Graph.h"
#include "vehicle.h"
#include <time.h>	
int max_capacity=200;
void Colony::update_phero_matrix(double &total_tour_distance)
{
	for (int i = 0; i < num_car; i++){
		double tour_distance = 0.0;
		calc_tour_distance(cars[i].tour, tour_distance);
		total_tour_distance += tour_distance;
		for (int j = 0; j < cars[i].tour.size(); j++){
			if(j+1 != cars[i].tour.size()){
				phero_matrix[cars[i].tour[j].idx][cars[i].tour[j+1].idx] = evap_rate * phero_matrix[cars[i].tour[j].idx][cars[i].tour[j+1].idx] + q / tour_distance;
				etha_matrix[cars[i].tour[j].idx][cars[i].tour[j+1].idx] = evap_rate*cars[i].car_capacity/max_capacity*etha_matrix[cars[i].tour[j].idx][cars[i].tour[j+1].idx]+q/tour_distance;
			} 
		}
	}
}
 void Colony:: update_Private_phero(double &total_tour_distance){
	for (int i = 0; i < num_car; i++){
		double tour_distance = 0.0;
		calc_tour_distance(cars[i].tour, tour_distance);
		total_tour_distance += tour_distance;
		for (int j = 0; j < cars[i].tour.size(); j++){
			if(j+1 != cars[i].tour.size()){
				phero_matrix[cars[i].tour[j].idx][cars[i].tour[j+1].idx] = evap_rate * phero_matrix[cars[i].tour[j].idx][cars[i].tour[j+1].idx] + q / tour_distance;
			} 
		}
	}
 }
void Colony:: update_Private_etha(double &total_tour_distance){
	for (int i = 0; i < num_car; i++){
		double tour_distance = 0.0;
		calc_tour_distance(cars[i].tour, tour_distance);
		total_tour_distance += tour_distance;
		for (int j = 0; j < cars[i].tour.size(); j++){
			if(j+1 != cars[i].tour.size()){
				etha_matrix[cars[i].tour[j].idx][cars[i].tour[j+1].idx] = evap_rate*cars[i].car_capacity/max_capacity*etha_matrix[cars[i].tour[j].idx][cars[i].tour[j+1].idx]+q/tour_distance;
			}
		}
	}
}
void Colony::move_ants()
{
	int car_idx = 0;
	while(!is_all_visited()){
		if(cars[car_idx].tour.empty()){
			cars[car_idx].add_node(&nodes[chose_depot()], distance_matrix);// add depot
		}
		std::vector< std::pair<int, int> > feasible_edges;
		feasible_edges.clear();
		while (feasible_edges.empty() && cars[car_idx].now_time <= 1250){
			for(int node_idx = num_depot; node_idx < num_node; node_idx++){
				if(!nodes[node_idx].visited){
					if(cars[car_idx].ok_capacity(nodes[node_idx])){
						if(cars[car_idx].ok_time(nodes[node_idx], distance_matrix)){
							feasible_edges.push_back(std::make_pair(cars[car_idx].now_idx, node_idx));
						}
					}
				}
			}
			if(feasible_edges.empty()){
				cars[car_idx].now_time += 10;// wait for the tw_open at the depot
			}
		}
		
		if(feasible_edges.empty()){// if still edges are not found, return depot
			if(car_idx + 1 < num_car){// check if the rest of vehicles exists
				if(cars[car_idx].now_idx >= num_depot){// in case the vehicle did not return back to the depot
					std::pair <double,int> near_depot[num_depot];
					for(int depot=0;depot<num_depot;depot++){
						near_depot[depot].first= distance_matrix[cars[car_idx].now_idx][depot];
						near_depot[depot].second=depot;
					}
					sort(near_depot,near_depot+num_depot-1);
					// std::cout<<"near"<<near_depot[0].first<<" "<<near_depot[0].second<<'\n';
					cars[car_idx].add_node(&nodes[near_depot[0].second],distance_matrix);
				}
				car_idx += 1;// assign next vehicle			
			}
			else{
				//std::cout << "cannot visit all customers this step." << std::endl;
				break;
			}
		}
		else{
			int next_node_idx;
			double ran=get_rand();
			if (ran < threshold){// choose randomly next node, this prevents local optimization
			int t=rand() % feasible_edges.size();
				next_node_idx = feasible_edges[t].second;
			}
			else{
			
				std::vector<double> ups, probs, cumulative_sum;
				double sum = 0.0;
				for (auto feasible_edge : feasible_edges){
					double up = calc_prob(feasible_edge.first, feasible_edge.second);
					sum += up;
					ups.push_back(up);
				}
				for (auto up : ups){
					probs.push_back(up / sum);
				}
				cumulative_sum.push_back(probs.front());
				for (int i = 0; i < probs.size() - 1; i ++){
					probs[i+1] += probs[i];
					cumulative_sum.push_back(probs[i+1]);
				}
				int candi_idx, best_idx;
				double candi_v;
				double best_v = 1000000;
				double r = get_rand();
				for (int x = 0; x < cumulative_sum.size(); x++){
					if (r <= cumulative_sum[x]){
						candi_idx = x;
						candi_v = cumulative_sum[x];
						if (candi_v < best_v){
							best_idx = candi_idx;
							best_v = candi_v;
						}
					}
				}
				if (best_v == 1000000){
						break;
				}
				next_node_idx = feasible_edges[best_idx].second;
			}
			cars[car_idx].add_node(&nodes[next_node_idx], distance_matrix);
		}
	}// while loop done
	if(cars[car_idx].now_idx >= num_depot){// in case the vehicle did not return back to the depot
		std::pair <double,int> near_depot[num_depot];
		for(int depot=0;depot<num_depot;depot++){
			near_depot[depot].first= distance_matrix[cars[car_idx].now_idx][depot];
			near_depot[depot].second=depot;
		}
		sort(near_depot,near_depot+num_depot-1);
		// std::cout<<"near"<<near_depot[0].first<<" "<<near_depot[0].second<<'\n';
		cars[car_idx].add_node(&nodes[near_depot[0].second],distance_matrix);
	}
}
bool Colony:: check_posible(int car_idx){
    std::vector< std::pair<int, int> > feasible_edges;
    int i;
		feasible_edges.clear();
		while (feasible_edges.empty() && cars[car_idx].now_time <= 1250){
			for(int node_idx = num_depot; node_idx < num_node; node_idx++){
				if(!nodes[node_idx].visited){
					if(cars[car_idx].ok_capacity(nodes[node_idx])){
						if(cars[car_idx].ok_time(nodes[node_idx], distance_matrix)){
							feasible_edges.push_back(std::make_pair(cars[car_idx].now_idx, node_idx));
						}
					}
				}
			}
		}
        if(feasible_edges.empty() && cars[car_idx].ok_time(nodes[i], distance_matrix)) return true;
        return false;
}
