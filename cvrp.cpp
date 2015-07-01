#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

struct Vertex{
	int num;			
	int x;		
	int y;				
	int demand;			
};

int vertex_num;						//the number of clients
double best_solution;				//the best solution	
int capacity;						//the capacity of each truck	

vector<Vertex> vertexs;				//the vector of clients vertexs	
int pop_size = 5;					//the population size
vector<vector<int> > population;	//the set of individuals in the population


void readFile(char dataFile[]){
	ifstream input;
	input.open(dataFile);
	if (input.is_open()){
		input >> vertex_num >> best_solution >> capacity;

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
		// cout << vertex_num <<" "<< best_solution<<" "<<capacity<<endl;

		// for (int i=0; i<= vertex_num; i++){
		// 	cout <<vertexs[i].num <<" "<< vertexs[i].x <<" "<<vertexs[i].y <<" "<<vertexs[i].demand<<endl;
		// }
	}
	else{
		cout << "open file failed!" <<endl;
	}
}


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

int main(){
	char dataFile[100] = "tai75a.dat";
	cout <<"please input the name of data file";
	//cin >> dataFile;
	readFile(dataFile);
	cout<<endl;

	initialize_population();

	for (int i = 0; i < pop_size; i++){
		cout << "individual " << i << ": ";
		for (int j = 0; j < vertex_num; j++){
			int temp = population[i][j];
			cout << vertexs[temp].num<< " ";
		}
		cout << endl;
		cout << endl;
	}
	system("pause");
}
