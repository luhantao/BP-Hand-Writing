#include <iostream>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <cmath>
#include <time.h>

using namespace std;

const int first = 784;
const int second = 100;
const int third = 10;
const double alpha = 0.35;

int input[first];						
int target[third];						
double weight1[first][second];			
double weight2[second][third];			
double output1[second];				
double output2[third];					
double delta1[second];		
double delta2[third];				
double b1[second];						
double b2[third];							

double test_num = 0.0;
double test_success_count = 0.0;

double f_(double x){
	return 1.0 / (1.0 + exp(-x));
}

void op1_(){
	for (int j = 0; j < second; j++){
		double sigma = 0;
		for (int i = 0; i < first; i++){
			sigma += input[i] * weight1[i][j]; 
		}
		double x = sigma + b1[j];
		output1[j] = f_(x);
	}
}

void op2_(){
	for (int k = 0; k < third; k++){
		double sigma = 0;
		for (int j = 0; j < second; j++){
			sigma += output1[j] * weight2[j][k];
		}
		double x = sigma + b2[k];
		output2[k] = f_(x);
	}
}

void dt2_(){
	for (int k = 0; k < third; k++){
		delta2[k] = (output2[k]) * (1.0 - output2[k]) * (output2[k] - target[k]);
	}
}

void dt1_(){
	for (int j = 0; j < second; j++){
		double sigma = 0;
		for (int k = 0; k < third; k++){
			sigma += weight2[j][k] * delta2[k];
		}
		delta1[j] = (output1[j]) * (1.0 - output1[j]) * sigma;
	}
}

void feedback_second(){
	for (int j = 0; j < second; j++){
		b1[j] = b1[j] - alpha * delta1[j];
		for (int i = 0; i < first; i++){
			weight1[i][j] = weight1[i][j] - alpha * input[i] * delta1[j];
		}
	}
}

void feedback_third(){
	for (int k = 0; k < third; k++){
		b2[k] = b2[k] - alpha * delta2[k];
		for (int j = 0; j < second; j++){
			weight2[j][k] = weight2[j][k] - alpha * output1[j] * delta2[k];
		}
	}
}

void initialize(){
	srand((int)time(0) + rand());
	for (int i = 0; i < first; i++){
		for (int j = 0; j < second; j++){
			weight1[i][j] = rand()%1000 * 0.001 - 0.5;
		}
	}	
	for (int j = 0; j < second; j++){
		for (int k = 0; k < third; k++){
			weight2[j][k] = rand()%1000 * 0.001 - 0.5;
		}
	}

	for (int j = 0; j < second; j++){
		b1[j] = rand()%1000 * 0.001 - 0.5;
	}
	for (int k = 0; k < third; k++){
		b2[k] = rand()%1000 * 0.001 - 0.5;
	}
}

void training(){
	FILE *image_train;
	FILE *image_label;
	image_train = fopen("../tc/train-images.idx3-ubyte", "rb");
	image_label = fopen("../tc/train-labels.idx1-ubyte", "rb");
	if (image_train == NULL || image_label == NULL){
		cout << "can't open the file!" << endl;
		exit(0);
	}

	unsigned char image_buf[784];
	unsigned char label_buf[10];
	
	int useless[1000];
	fread(useless, 1, 16, image_train);
	fread(useless, 1, 8, image_label);

	int cnt = 0;
	cout << "Start training..." << endl;
	//60000 times
	while (!feof(image_train) && !feof(image_label)){
		memset(image_buf, 0, 784);
		memset(label_buf, 0, 10);
		fread(image_buf, 1, 784, image_train);
		fread(label_buf, 1, 1, image_label);

		//initialize the input by 28 x 28 (0,1)matrix of the images
		for (int i = 0; i < 784; i++){
			if ((unsigned int)image_buf[i] < 128){
				input[i] = 0;
			}
			else{
				input[i] = 1;
			}
		}

		//initialize the target output
		int target_value = (unsigned int)label_buf[0];
		for (int k = 0; k < third; k++){
			target[k] = 0;
		}
		target[target_value] = 1;

		//get the output and start training
		op1_();
		op2_();
		dt2_();
		dt1_();
		feedback_second();
		feedback_third();

		cnt ++;
		if (cnt % 1000 == 0){
			cout << "training image: " << cnt << endl;
		}
	}
	cout << endl;
}


void testing(){
	FILE *image_test;
	FILE *image_test_label;
	image_test = fopen("../tc/t10k-images.idx3-ubyte", "rb");
	image_test_label = fopen("../tc/t10k-labels.idx1-ubyte", "rb");
	if (image_test == NULL || image_test_label == NULL){
		cout << "can't open the file!" << endl;
		exit(0);
	}

	unsigned char image_buf[784];
	unsigned char label_buf[10];
	
	int useless[1000];
	fread(useless, 1, 16, image_test);
	fread(useless, 1, 8, image_test_label);

	while (!feof(image_test) && !feof(image_test_label)){
		memset(image_buf, 0, 784);
		memset(label_buf, 0, 10);
		fread(image_buf, 1, 784, image_test);
		fread(label_buf, 1, 1, image_test_label);

		//initialize the input by 28 x 28 (0,1)matrix of the images
		for (int i = 0; i < 784; i++){
			if ((unsigned int)image_buf[i] < 128){
				input[i] = 0;
			}
			else{
				input[i] = 1;
			}
		}

		//initialize the target output
		for (int k = 0; k < third; k++){
			target[k] = 0;
		}
		int target_value = (unsigned int)label_buf[0];
		target[target_value] = 1;
		
		//get the ouput and compare with the targe
		op1_();
		op2_();

		double max_value = -99999;
		int max_index = 0;
		for (int k = 0; k < third; k++){
			if (output2[k] > max_value){
				max_value = output2[k];
				max_index = k;
			}
		}

		//output == target
		if (target[max_index] == 1){
			test_success_count ++;
		}
		
		test_num ++;

		if ((int)test_num % 1000 == 0){
			cout << "test num: " << test_num << "  success: " << test_success_count << endl;
		}
	}
	cout << endl;
	cout << "The success rate: " << test_success_count / test_num << endl;
}

int main(){
	initialize();
	training();
	testing();

	system("pause");
}