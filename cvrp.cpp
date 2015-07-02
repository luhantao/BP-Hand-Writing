#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

using namespace std;

struct Vertex{
	int num;			
	int x;		
	int y;				
	int demand;			
};

int vertex_num;							//the number of clients
int capacity;							//the capacity of each truck	

int pop_size;						//the population size
double transform_probability = 0.200;	//the probability of individual transform
vector<Vertex> vertexs;					//the vector of clients vertexs	
vector<vector<int> > population;		//the set of individuals in the population

double cur_best_distance;				//the best solution in current generation
vector<int> cur_best_individual;		//the best individual in current generation 

double best_solution;					//the best solution	in all generations
vector<int> best_individual;			//the best individual in all generations


void readFile(char dataFile[]){
	ifstream input;
	input.open(dataFile);
	if (input.is_open()){
		input >> vertex_num >> best_solution >> capacity;
		best_solution = 99999999;

		//initialize the vector
		Vertex temp;
		for(int i = 0; i <= vertex_num; i++){
			vertexs.push_back(temp);
		}
		//depot coordinate (0,0)
		vertexs[0].num = 0;
		input >> vertexs[0].x >> vertexs[0].y;

		for (int i = 1; i <= vertex_num; i++){
			try{
				input >> vertexs[i].num;
				input >> vertexs[i].x;
				input >> vertexs[i].y;
				input >> vertexs[i].demand;
			}
			catch(string &err){
				cout << err <<endl;
			}
		}
	}
	else{
		cout << "open file failed!" <<endl;
	}
}

//randomly initialize the population, according to the pop_size
void initialize_population(){
	//truely random number
	srand((int)time(0) + rand());
	vector <int> sort_vector;
	for (int i = 1; i <= vertex_num; i++){
		sort_vector.push_back(i);
	}
	for (int i = 0; i < pop_size; i++){
		vector <int> temp = sort_vector;
		int cnt = 0;
		while (cnt < vertex_num*2){
			int first = rand()% vertex_num;
			int second = rand()% vertex_num;
			while (first == second){
				second = rand()% vertex_num;
			}
			int exchange = temp[first];
			temp[first] = temp[second];
			temp[second] = exchange;

			cnt ++;
		}
		population.push_back(temp);
	}	
}

vector <int> add_depot_coordinate(vector <int> individual){
	vector <int> add_depot;
	add_depot.push_back(0);
	int capacity_used = 0;

	//add zero depot in the individual vector, like: {0,3,2,0,1,0}
	for (int i = 0; i < individual.size(); i++){
		if (capacity_used + vertexs[individual[i]].demand < capacity){
			add_depot.push_back(individual[i]);
			capacity_used += vertexs[individual[i]].demand;
		}
		else{
			add_depot.push_back(0);
			add_depot.push_back(individual[i]);
			capacity_used = vertexs[individual[i]].demand;
		}
	}
	add_depot.push_back(0);
	return add_depot;
}

double caculate_individual_distance(vector <int> individual){
	vector <int> add_depot = add_depot_coordinate(individual);

	//caculate the distance of the new vector with zero
	double distance;
	for (int i = 1; i < add_depot.size(); i++){
		int first_x = vertexs[add_depot[i-1]].x;
		int first_y = vertexs[add_depot[i-1]].y;
		int second_x = vertexs[add_depot[i]].x;
		int second_y = vertexs[add_depot[i]].y;

		double edge_length = pow((first_x - second_x), 2) + pow((first_y - second_y), 2);
		edge_length = pow(edge_length, 0.5);
		distance += edge_length;
	}
	return distance * 1.0;
}

void choose_individuals(){
	double population_fitness = 0;
	vector <double> individuals_fitness;
	vector <double> accumulate_probability;

	//caculate all individuals' fitness, record them and caculate the fitness of whole population
	cur_best_distance = 99999999;
	for (int i = 0; i < pop_size; i++){
		double cur_distance = caculate_individual_distance(population[i]);
		if (cur_distance < cur_best_distance){
			cur_best_distance = cur_distance;
			cur_best_individual = population[i];
		}
		if (cur_distance < best_solution){
			best_solution = cur_distance;
			best_individual = population[i];
		}

		double cur_fitness = 1.0 / cur_distance;
		individuals_fitness.push_back(cur_fitness);
		population_fitness += cur_fitness;
	}

	//caculate the chosen probability of each individual, and accumulate them
	for (int i = 0; i < pop_size; i++){
		double probability = individuals_fitness[i] / population_fitness;
		if (i != 0){
			accumulate_probability.push_back(accumulate_probability[i-1] + probability);
		}
		else{
			accumulate_probability.push_back(probability);
		}
	}

	//choose the number of 'pop_size' new individuals of population
	srand((int)time(0) + rand());
	vector <vector<int> > newPopulation;
	for (int i = 0; i < pop_size; i++){
		double randomly = rand()%10000 * 0.0001;
		//50% of chance push the best individual
		if (i == 0 && randomly < 0.5){
			newPopulation.push_back(best_individual);
		}
		else{
			for (int j = 0; j < pop_size; j++){
				if (randomly < accumulate_probability[j]){
					newPopulation.push_back(population[j]);
					break;
				}
			}
		}
	}
	//use the new population to replace the old one
	population.clear();
	population = newPopulation;
}

void individuals_transform(){
	srand((int)time(0) + rand());
	//use the operation of 'Inver-Over'
	for (int i = 0; i < pop_size; i++){
		double start_num = rand()%75 + 1;
		double end_num;
		int start_index, end_index;
		for (int j = 0; j < vertex_num; j++){
			if (population[i][j] == start_num){
				start_index = j;
				break;
			}
		}

		//two method of operation
		double transform = rand()%10000 * 0.0001;
		//likely mutation
		if (transform < transform_probability){
			end_num = rand()%75 + 1;
			while (start_num == end_num){
				end_num = rand()%75 + 1;
			}
			for (int j = 0; j < vertex_num; j++){
				if (population[i][j] == end_num){
					end_index = j;
					break;
				}
			}
		}
		//likely crossover
		else{
			int other = rand() % pop_size;
			while (other == i){
				other = rand() % pop_size;
			}
			for (int j = 0; j < vertex_num; j++){
				if (population[other][j] == start_num){
					if (j == vertex_num - 1){
						end_num = population[other][j-1];
					}
					else{
						end_num = population[other][j+1];
					}
					break;
				}
			}
			for (int j = 0; j < vertex_num; j++){
				if (population[i][j] == end_num){
					end_index = j;
					break;
				}
			}
		}
		if (start_index > end_index){
			int temp = start_index;
			start_index = end_index;
			end_index = temp;
		}

		//reverse the vertexs in individual between start_index and end_index
		vector <int> reverse;
		for (int j = start_index + 1; j <= end_index; j++){
			reverse.push_back(population[i][j]);
		}
		int reverse_index = reverse.size() - 1;
		for (int j = start_index + 1; j <= end_index; j++){
			population[i][j] = reverse[reverse_index];
			reverse_index --;
		}
	}
}

int main(){
	char dataFile[100];
	int max_generation;
	cout << "please input the name of data file: ";
	cin >> dataFile;
	cout << endl;

	cout << "please input the size of population: ";
	cin >> pop_size;
	cout <<	endl;

	cout << "please input the number of generation: ";
	cin >> max_generation;
	cout <<	endl;

	readFile(dataFile);
	initialize_population();

	int generation = 1;
	while (generation < max_generation){
		choose_individuals();
		individuals_transform();

		cout <<"the best in current generation: "<<cur_best_distance<< endl;
		generation ++; 
	}

	cout << endl;
	cout << "The cost of the best solution is: "<< best_solution<< endl;
	cout << endl;
	cout << "The vertexs' order of the best solution is: "<< endl;
	vector <int> result = add_depot_coordinate(best_individual);
	for (int i = 0; i < result.size(); i++){
		cout << result[i]<<" ";
	}
	cout <<	endl;
	system("pause");
}
