#include <iostream>
#include <fstream>
using namespace std;

struct Vertex{
	int num;				//顶点号
	int x;					//横坐标x
	int y;					//纵坐标y
	int demand;				//客户需求量
};

int vertex_num;				//顶点个数
double best_solution;		//最优解
int capacity;				//货车最大装载量
Vertex vertex[1000];		//顶点数组

//读入文件，并初始化全局变量
void readFile(char dataFile[]){
	ifstream input;
	input.open(dataFile);
	if (input.is_open()){
		input >> vertex_num >> best_solution >> capacity;
		//仓库原点
		vertex[0].num = 0;
		input >> vertex[0].x >> vertex[0].y;
		//读入顶点数据
		for (int i=1; i<=vertex_num; i++){
			try{
				input >> vertex[i].num;
				input >> vertex[i].x;
				input >> vertex[i].y;
				input >> vertex[i].demand;
			}
			catch(string &err){
				cout << err <<endl;
			}
		}
		/*
		cout << vertex_num <<" "<< best_solution<<" "<<capacity<<endl;

		for (int i=0; i<= vertex_num; i++){
			cout <<vertex[i].num <<" "<< vertex[i].x <<" "<<vertex[i].y <<" "<<vertex[i].demand<<endl;
		}
		*/


	}
	else{
		cout << "open file failed!" <<endl;
	}
}

int main(){
	//输入数据文件名字
	char dataFile[100] = "tai75a.dat";
	cout <<"输入数据文件名字：";
	//cin >> dataFile;
	readFile(dataFile);

	system("pause");
}
