#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath> // sqrt, pow
#include <iomanip> // setprecision 
#include "Graph.h"
#include "vehicle.h"
#include "colony.h"
void Graph::calc_node_distance()
{
	
	for(int i = 0; i < num_node; i++){
		for(int j = i + 1; j < num_node; j++){
			distance_matrix[i][j] = distance_matrix[j][i] = sqrt(pow(nodes[i].x - nodes[j].x, 2) + pow(nodes[i].y - nodes[j].y, 2));
		}
	}
	// freopen("probs/Multi_depot/test/out.txt","r",stdin);
	// for(int i = 0; i <= 34; i++){
	// 	for(int j = 0; j <= 34; j++){
	// 		std::cin>>distance_matrix[i][j];
	// 	}
	// }
}

Graph::Graph(std::vector<std::vector<int> > param,
				int car_capacity,int bike_capacity, int num_car,int num_bike,int num_depot)
{    
	this->num_depot = num_depot;
	this->num_car = num_car;
	this->num_node = param.size(); 
	std::cout << "constructor!" <<std:: endl;
	std::cout << "num_car:" << num_car << " car_capacity:" \
		<< car_capacity << " num_node(depot + customer):" << num_node << std::endl;
	std::cout << "Num_depot: "<< num_depot <<std::endl;

	nodes = new Node[num_node];
	cars = new Car[num_car];

	for(int i = 0; i < num_node; i++){
		nodes[i].idx = param[i][0];
		nodes[i].x = param[i][1];
		nodes[i].y = param[i][2];
		nodes[i].demand = param[i][3];
		nodes[i].visited = false;
		//time windows
		nodes[i].tw_open = param[i][4];
		nodes[i].tw_close = param[i][5];
		nodes[i].unload_time = (int)(0.2 * (param[i][5] - param[i][4]));
		nodes[i].service_time= param[i][6];
		//std::cout<<nodes[i].idx<<" "<<nodes[i].demand<<std::endl;
	}

	for(int i = 0; i < num_bike; i++){
		cars[i].car_capacity = bike_capacity;
		cars[i].now_load = 0;
		cars[i].now_idx = 0;
		cars[i].car_cost= 30;
		//time windows
		cars[i].car_speed = 10;
		cars[i].tour_cost = 0;
		cars[i].now_time = 0;
	}
	for(int i = num_bike; i < num_car; i++){
		cars[i].car_capacity = car_capacity;
		cars[i].now_load = 0;
		cars[i].now_idx = 0;
		cars[i].car_cost= 60;
		cars[i].tour_cost = 0;
		cars[i].car_speed = 10;
		cars[i].now_time = 0;
	}
	distance_matrix = new double* [num_node]; 
	for(int i = 0; i < num_node; i++){
		distance_matrix[i] = new double [num_node];
		for(int j = 0; j < num_node; j++){
			distance_matrix[i][j] = 0;// initialization all elements in a row
		}
	}
	calc_node_distance();
}

Graph::~Graph()
{ 
	std::cout << "destructor!" << std::endl;
	delete[] nodes;
	nodes = nullptr;
	delete[] cars;
	cars = nullptr;
	for(int i = 0; i < num_node; i++){
		delete[] distance_matrix[i];
		distance_matrix[i] = nullptr;
	}
	delete[] distance_matrix;
	distance_matrix = nullptr;
}

void Graph::show_distance_matrix() const
{
	std::cout << num_node << "*" << num_node << " distance matrix." << std::endl;
	for(int i = 0; i < num_node; i++){
		for(int j = 0; j < num_node; j++){
			std::cout <<std:: fixed << std::setprecision(1) << distance_matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

bool Graph::is_all_visited() const
{
	for(int i = num_depot; i < num_node; i++){
		if(!nodes[i].visited){
			return false;
		}
	}
	return true;
}

void Graph::run_GreedyAlgorithm()
{
	int car_idx = 0;
	while(!is_all_visited()){
		int best_node_idx;
		double min_distance = 100000;
		bool is_return_depot = true;
		if(cars[car_idx].tour.empty()){
			cars[car_idx].add_node(&nodes[0], distance_matrix);// nodes[0] is depot
		}

		for(int node_idx = 1; node_idx < num_node; node_idx++){
			if(!nodes[node_idx].visited){//neu node chua dc tham
				if(cars[car_idx].ok_capacity(nodes[node_idx])){// check capacity
					if(cars[car_idx].ok_time(nodes[node_idx], distance_matrix)){//check time
						double tmp_distance = distance_matrix[cars[car_idx].now_idx][node_idx];
						if(tmp_distance < min_distance){
							min_distance = tmp_distance;
							is_return_depot = false;
							best_node_idx = node_idx;
						}
					}
				}
			}
		}
		if(!is_return_depot){
			cars[car_idx].add_node(&nodes[best_node_idx], distance_matrix);
		}
		else{// if edges are not found, return depot
			if(car_idx + 1 < num_car){// check if the rest of vehicles exists
				if(cars[car_idx].now_idx != 0){// in case the vehicle did not return back to the depot
					cars[car_idx].add_node(&nodes[0], distance_matrix);
				}
				car_idx++;// assign next vehicle
			}
			else{
				std::cout << "Cannot solve this by Greedy algorithm." << std::endl;
				break;// exit(0);
			}
			
		}
	}//while loop done
	if(cars[car_idx].now_idx != 0){// in case the vehicle did not return back to the depot
				cars[car_idx].add_node(&nodes[0], distance_matrix);
	}
	std::cout << "algorithm done." << std::endl;
}

void Graph::calc_tour_distance(std::vector<Node>tour, double &tour_distance) const
{
	if(tour.size() > 2){
		for(int j = 0; j < tour.size() - 1; j++){
			tour_distance += distance_matrix[tour[j].idx][tour[j+1].idx];		
		}
	}
}

void Graph::show_each_car_tour() const
{
	double total_tour_distance = 0.0;
	int total_visited_customer = 0;
	double total_tour_cost =0;
	double total_penalty_cost =0;
	int c=0;
	for(int i = 0; i < num_car; i++){
		if(cars[i].tour.size() > 1){
			std::cout << "vehicle" << i << " tour: ";
			double tour_distance = 0.0;
			std::pair <double,int> near_depot[num_depot];
				for(int depot=0;depot<num_depot;depot++){
				near_depot[depot].first= distance_matrix[cars[i].tour[1].idx][depot];
				near_depot[depot].second=depot;
				}
				sort(near_depot,near_depot+num_depot-1);
				cars[i].tour[0].idx=near_depot[0].second;
			for(int j = 0; j < cars[i].tour.size(); j++){
				std::cout << cars[i].tour[j].idx << " ";
				//total_penalty_cost +=nodes[cars[i].tour[j].idx].penalty_cost;
			}
			calc_tour_distance(cars[i].tour, tour_distance);
			total_tour_distance += tour_distance;
			total_tour_cost+=tour_distance*cars[i].car_cost;			
			std::cout << " " <<std:: fixed <<std:: setprecision(1) << tour_distance << "km";
			std::cout << ", visited customer:" << cars[i].tour.size()-2;
			std::cout << ", now_time:" << cars[i].now_time;
			std::cout << ", now_load:" << cars[i].now_load << "/" << cars[i].car_capacity <<std:: endl;
			total_visited_customer+= cars[i].tour.size()-2;
		}
		else{
			std::cout << "vehicle" << i << ":"<<" now time "<<cars[i].now_time<<", not used";
			std::cout << " now_node: "<<cars[i].now_idx<<std::endl;
		}
	}
	for(int i=num_depot;i<num_node;i++){
		if(!nodes[i].visited)total_penalty_cost+=10*nodes[i].demand;
	}
	std::cout << "total visited customer:" << total_visited_customer << "/" << num_node - num_depot << std::endl;// -1 denotes not counting the depot
	std::cout << "total distance:" << total_tour_distance << "km" << std::endl;
	std::cout << "total cost:" << 1000*(total_tour_cost+total_penalty_cost) << "đ" << std::endl;
	std::cout << "total penalty cost:" << 1000*total_penalty_cost << "đ" << std::endl;
}

void Graph::show_node_info() const
{
	std::cout <<std:: endl << "idx,x,y,demand,tw_open,tw_close,unload_time" << std::endl;
	for(int i = 0; i < num_node; i++){
		std::cout << nodes[i].idx << " " << nodes[i].x << " " << nodes[i].y << " " << nodes[i].demand << \
			" " << nodes[i].tw_open << " " << nodes[i].tw_close << " " << nodes[i].unload_time <<std:: endl;
	}
}
// int Graph::total(int m,int s){
// 	if(s==0) return m;
// 	return total(m,0)+s;
// }