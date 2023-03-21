#include <bits/stdc++.h>
#include "Graph.h"
#include "vehicle.h"
#include "colony.h"
#include "fileloader.h"
//#define nullptr 0
int main(int argc, char const *argv[]){

	// if (argc < 2){
	// 	std::cerr << "argc: " << argc << "should be >= 2";
	// 	return -1;
	// }
	int n=5;
	int min=1000000;
	//std::cin>>n;
	freopen("output.txt","w",stdout);
	// while(n>0){
		const clock_t start_time = clock();
		std::vector<std::vector<int> > param;
		int car_capacity, num_car,num_depot,num_bike,bike_capacity=0;
		car_capacity=0;
		num_car=0;
		file_loader("probs/Multi_depot/C102.txt", param, car_capacity,bike_capacity, num_car,num_bike, num_depot);
		Graph* graph = new Graph(param, car_capacity,bike_capacity, num_car, num_bike, num_depot);
		// graph->run_GreedyAlgorithm();
		// graph->show_each_car_tour();
		// graph->show_node_info();
		// graph->show_distance_matrix();
		delete graph;
		graph = nullptr;
		Colony* colony = new Colony(param, car_capacity,bike_capacity, num_car, num_bike);
		colony->run_ACO();
		//colony->show_distance_matrix();
		
		delete colony;
		colony = nullptr;
		const clock_t end_time = clock();
		std::cout << (end_time - start_time)/ double(CLOCKS_PER_SEC) << "sec" << std::endl;
		// n--;
	//}
	return 0;
}
