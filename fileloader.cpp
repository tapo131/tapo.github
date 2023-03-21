#include "fileloader.h"

void file_loader (const char *filename, 
				std::vector<std::vector<int> > &param, 
				int &car_capacity,int &bike_capacity, int &num_car,int & num_bike,int &num_depot)
{
	
	std::ifstream ifs(filename, std::ios::in);
	if(!ifs.is_open()){
		std::cerr << "Error, cannot open file, check argv: " << filename << std::endl;
		std::exit(1); 
	}
   std::string line;
   for(int i = 0; i < 4; i++){
   	std::getline(ifs, line);
   }
   std::getline(ifs, line);
   std::stringstream ss(line);
   ss >> num_car >> car_capacity>>num_depot>>num_bike>>bike_capacity;
   while (std::getline(ifs, line)){
   	//std::cout << line.size() << std::endl;
	   if(66 <= line.size() && line.size() <= 75){
	    	std::stringstream ss1(line);
	      std::vector<int> tmp_vec;
	      int tmp;
	      while (ss1 >> tmp){
	      	tmp_vec.push_back(tmp);
			//printf("%f",tmp);
	      }
	      param.push_back(tmp_vec);
		  //std::cout<<std::endl;
	   }
   }
   ifs.close();
}
