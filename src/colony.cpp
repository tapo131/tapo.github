#include <iostream>
#include <vector>
#include <random> // rand_dev
#include <algorithm> // std::max_element
#include "colony.h"
#include "Graph.h"
#include "vehicle.h"
#include <time.h>	
const double Colony::init_phero = 1.0;// khởi tạo pheromone
const double Colony::evap_rate = 0.95;// độ bay hơi khi pheromone updates
const double Colony::alpha = 3.0;// prob_next_city
const double Colony::beta = 5.0;// prob_next_city
const double Colony::q = 1.0;// hệ số chất lượng (chia cho tour length)
double Colony::threshold = 0.5;// mức lựa chọn đi đến node random hay là theo mùi
const double Colony::decay_rate = 0.95;	 // do phan ra
double INFIN = 100000;


void Colony::run_ACO()
{
	double min_distance = INFIN;
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
void Colony::reset_at_each_step()
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

void Colony::update_threshold()
{
	threshold *= decay_rate;
}

void Colony::update_phero_matrix(double &total_tour_distance)
{
	for (int i = 0; i < num_car; i++){
		double tour_distance = 0.0;
		calc_tour_distance(cars[i].tour, tour_distance);
		total_tour_distance += tour_distance;
		for (int j = 0; j < cars[i].tour.size(); j++){
			if(j+1 != cars[i].tour.size()){
				phero_matrix[cars[i].tour[j].idx][cars[i].tour[j+1].idx] = evap_rate * phero_matrix[cars[i].tour[j].idx][cars[i].tour[j+1].idx] + q / tour_distance;
				//pheromatrix: vết mùi 
				//evap_rate * phero_matrix[cars[i].tour[j].idx][cars[i].tour[j+1].idx]: phần vết cũ
				//q/tour_distance: phần vết cập nhật
			} 
		}
	}
}
int Colony::chose_depot(){
	int first_depot = rand()%num_depot;
	//nodes[first_depot].service
	return first_depot;
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
			else{// follow heuristc and pheromone formula, choose next node
			
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
				double best_v = INFIN;
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
				if (best_v == INFIN){
						break;
				}
				next_node_idx = feasible_edges[best_idx].second;
				
				// search algorithm
				// double rand = get_rand();
				// // std::sort(cumulative_sum.begin(), cumulative_sum.end());
				// for (auto itr = cumulative_sum.begin(); itr != cumulative_sum.end(); itr++){
				// 	if (rand_num < *itr){
				// 		int next_node_idx = itr;
				// 		break;
				// 	}
				// }
				// auto iter = std::max_element(cumulative_sum.begin(), cumulative_sum.end());
				// next_node_idx = std::distance(cumulative_sum.begin(), iter);
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

double Colony::calc_prob(int now_node_idx, int next_node_idx) const
{
	double ETAij = pow (etha_matrix[now_node_idx][next_node_idx], beta);
	double TAUij = pow (phero_matrix[now_node_idx][next_node_idx], alpha);
	return ETAij * TAUij;
}


double Colony::get_rand() const
{
	std::srand(time(0));
    std::random_device rand_dev;
    std::mt19937 generator(time(0));
    std::uniform_real_distribution<double> distr(0, 1);
    return distr(generator);
}

Colony::Colony(std::vector<std::vector<int> > param,
				int car_capacity,int bike_capacity, int num_car,int num_bike,std::vector<std::vector<double> >param1) : Graph(param, car_capacity,bike_capacity, num_car,num_bike,num_depot,param1)
{
	std::cout << "constructor!!" << std::endl;
	phero_matrix = new double* [num_node];
	etha_matrix = new double* [num_node]; 
	for(int i = 0; i < num_node; i++){
		phero_matrix[i] = new double [num_node];
		etha_matrix[i] = new double [num_node];
		for(int j = 0; j < num_node; j++){
			phero_matrix[i][j] = 0;// initialize all elements in a row
			etha_matrix[i][j] = 0;
		}
	}
	for(int i = 0; i < num_node; i++){
		for(int j = i + 1; j < num_node; j++){
			phero_matrix[i][j] = phero_matrix[j][i] = init_phero; //tạo mùi
			etha_matrix[i][j] = etha_matrix[j][i] = q / distance_matrix[i][j];//tạo thông tin heuristic
		}
	}
}

Colony::~Colony()
{ 
	std::cout << "destructor!!" << std::endl;
	for(int i = 0; i < num_node; i++){
		delete[] phero_matrix[i];
		phero_matrix[i] = nullptr;
		delete[] etha_matrix[i];
		etha_matrix[i] = nullptr;
	}
	delete[] phero_matrix;
	phero_matrix = nullptr;
	delete[] etha_matrix;
	etha_matrix = nullptr;
}
