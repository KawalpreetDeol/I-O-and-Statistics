//#includes
#include <fstream>
#include <cmath>
#include <iostream>

//# Defines
#define isNaN(X) (X != X)  

// Struct definitions 
struct Student{
	int studentID;
	int grade;
};

struct Dataset{
	int numStudents;
	Student* students;
};

struct Rejects{
	int numRejects;
	int* rejects;
};

struct Mode{
	int numModes;
	int* modes;
};

struct Statistics{
	int   minimum;
	float average;
	int   maximum;
	float popStdDev;
	float smplStdDev;
	Mode  mode;
	int   histogram[10];
};

// Function declarations
int readCSV(const char filename[],const int minAcceptableID, 
				const int maxAcceptableID,Dataset& data, 
				Rejects& rejects);

int computeStatistics(Dataset& data, Statistics& stats);

int writeCSV(const char filename[], const Statistics& stats);

// Function definitions
using namespace std;
int min(Dataset& data, Statistics& stats);
float avg(Dataset& data, Statistics& stats);
int max(Dataset& data, Statistics& stats);
float popSDev(Dataset& data, Statistics& stats);
float smplSDev(Dataset& data, Statistics& stats);
int getMode(Dataset& data, Statistics& stats);
int histogramCreator(Dataset& data, Statistics& stats);
void selection(Dataset& data, const int index);

int readCSV(const char filename[],const int minAcceptableID, 
				const int maxAcceptableID,Dataset& data, 
				Rejects& rejects){
	//opening file
	ifstream infile;
	infile.open(filename, ios::in);

	if (!infile.is_open()){
    	return -1;
  	}

	char input[101];
	char IDBuffer[101];
	char gradeBuffer[101];
	
	data.students = new Student[1000];
	int IDValue;
	int gradeValue;

	int bufferCount = 0;
	int commaCheck = 0;

	rejects.rejects = new int[10];
	bool ifReject = false;
	int rejectionCount = 0;

	int valuesCount = 0;

	int lineCount = 0;
	int spaceCheck = 0;

	while(!infile.eof()){

		infile.getline(input, 100);
		//to truncate the char
		lineCount++;
		for (int i = 0; input[i] != 0; i++){

			if ((input[i] == '!') || (input[i] == '@') || (input[i] == '#') || (input[i] == '$') || (input[i] == '%') || (input[i] == '^') || (input[i] == '&') || (input[i] == '*') || (input[i] == '(') || (input[i] == ')') || (input[i] == '?') || (input[i] == '<') || (input[i] == '>') || (input[i] == '|')){
				ifReject = true;
				break;
			}
			else if(((input[i] <= '9') && (input[i] >= '0')) && (commaCheck == 0)){
				IDBuffer[bufferCount] = input[i];
				bufferCount++;
				if(spaceCheck > 0){
					ifReject = true;
				}
			}
			else if (input[i] == ','){
				commaCheck++;
				if (bufferCount == 0){
					ifReject = true;
					break;
				}
				bufferCount = 0;
				spaceCheck = 0;

				if(commaCheck > 1){
					ifReject = true;
					break;
				}
			}
			else if (((input[i] <= '9') && (input[i] >= '0')) && (commaCheck == 1)){
				gradeBuffer[bufferCount] = input[i];
				bufferCount++;
				if(spaceCheck > 0){
					ifReject = true;
				}
			}
			else if ((input[i] == ' ') || (input[i] == '\t')){
				if (bufferCount > 0){
					spaceCheck++;
				}
			}
			else if(input[i] == '\n'){

			}
			
		} //for loop ends
		if ((commaCheck == 0) && (bufferCount > 0)){
			ifReject = true;
		}
		//covert char to int
		if (ifReject == false){
			sscanf(IDBuffer, "%d", &IDValue);
			sscanf(gradeBuffer, "%d", &gradeValue);
			if ((IDValue < minAcceptableID) || (IDValue > maxAcceptableID) || (gradeValue < 0) || (gradeValue > 100) || (isnan(gradeValue)) || ((isnan(IDValue)))){
			ifReject = true;
			}
		}
		if ((bufferCount == 0) && (commaCheck == 1)){
			ifReject = true;
		}
		//store in struct
		if(ifReject == false){
			data.students[valuesCount].studentID = IDValue;
			data.students[valuesCount].grade = gradeValue;
			valuesCount++;
		}
		else if(ifReject == true){
			rejects.rejects[rejectionCount] = lineCount;
			rejectionCount++;
		}
		else if((commaCheck == 0) && (bufferCount == 0)){
		}
		
		for(int k = 0; k < 100; k++){
			IDBuffer[k] = 0;
			gradeBuffer[k] = 0;
		}

		commaCheck = 0;
		bufferCount = 0;
		ifReject = false;
		IDValue = 0;
		gradeValue = 0;

	}
	 // end of while loop
	//Return function
	//infile.close();
	if (rejectionCount >= 10){
		return -1;
	}
	if ((rejectionCount == 0)){
		data.numStudents = valuesCount;
		rejects.numRejects = rejectionCount;
		return 0;
	}
	else if((valuesCount > 0) && (rejectionCount > 0)){
		data.numStudents = valuesCount;
		rejects.numRejects = rejectionCount;
		return rejectionCount;
	}
	else{
		return -1;
	}
}

int computeStatistics(Dataset& data, Statistics& stats){
	if (data.numStudents == 0){
		return -1;
	}
	selection(data, 0);
	max(data, stats);
	min(data, stats);
	avg(data, stats);
	histogramCreator(data, stats);
	getMode(data, stats);
	if (data.numStudents == 1){
		return -1;
	}
	popSDev(data, stats);
	smplSDev(data, stats);
	return 0;
}

int writeCSV(const char filename[], const Statistics& stats){
	ofstream output;
	if (stats.mode.numModes == 0){
		return -1;
	}
	char arr[100];
	int nameCount = 0;
	int periodCount = 0;
	for(int i = 0; filename[i] != 0; i++){
		arr[i] = filename[i];
		nameCount++;
	}
	for(int i = 0; i < nameCount; i++){
		if(arr[i] == '.'){
			periodCount++;
		}
		if (periodCount > 1){
			return -1;
		}
	}
	for(int i = 0; i < nameCount; i++){
		if((arr[i] == '.') && (arr[i+1] == 'c') && (arr[i+2] == 's') && (arr[i+3] == 'v')){
			arr[i+1] = 's';
			arr[i+2] = 't';
			arr[i+3] = 'a';
			arr[i+4] = 't';
			i = nameCount;
		}
	}

	output.open(arr, ios::out);
	output << "Minimum: " << stats.minimum << endl;
	output << "Average: " << stats.average << endl;
	output << "Maximum: " << stats.maximum << endl;
	output << "Population Standard Deviation: " << stats.popStdDev << endl;
	output << "Sample Standard Deviation: " << stats.smplStdDev << endl;

	output << "Modes: ";
	for(int i = 0; i < stats.mode.numModes; i++){
		output << stats.mode.modes[i];
		if (i != (stats.mode.numModes - 1)){
			output << ", ";
		}
	}
	output << endl;

	output << "Histogram: " << endl;
	output << "[0-9]: " << stats.histogram[0] << endl;
	output << "[10-19]: " << stats.histogram[1] << endl;
	output << "[20-29]: " << stats.histogram[2] << endl;
	output << "[30-39]: " << stats.histogram[3] << endl;
	output << "[40-49]: " << stats.histogram[4] << endl;
	output << "[50-59]: " << stats.histogram[5] << endl;
	output << "[60-69]: " << stats.histogram[6] << endl;
	output << "[70-79]: " << stats.histogram[7] << endl;
	output << "[80-89]: " << stats.histogram[8] << endl;
	output << "[90-100]: " << stats.histogram[9];

	output.close();
	
	return 0;
}


int histogramCreator(Dataset& data, Statistics& stats){
	int count = 0;
	for(int j = 0; j < data.numStudents; j++){
		count = 0;
		for(int i = 0; i < 10; i++){
			if((data.students[j].grade >= (i*10)) && (data.students[j].grade <= (((((i+1)*10) - 1) + i/9)))){
				stats.histogram[count]++;
			}
			count++;
		}
	}
	return 0;
}

void selection(Dataset& data, const int index){
	if (index == data.numStudents){
		return;
	}
	int min = 0;
	int IDtmp = 0;
	int gradetmp = 0;
	for(int i = (index + 1); i < data.numStudents; i++){
		if (data.students[index].grade > data.students[i].grade){
			//sorting grades
			gradetmp = data.students[i].grade;
			data.students[i].grade = data.students[index].grade;
			data.students[index].grade = gradetmp;

			//sorting student IDs
			IDtmp = data.students[i].studentID;
			data.students[i].studentID = data.students[index].studentID;
			data.students[index].studentID = IDtmp;
		}
	}
	selection(data, index + 1);
}

int max(Dataset& data, Statistics& stats){
	stats.maximum = data.students[data.numStudents - 1].grade;
	return 0;
}

int min(Dataset& data, Statistics& stats){
	stats.minimum = data.students[0].grade;
	return 0;
}

float avg(Dataset& data, Statistics& stats){
	int sum = 0;
	for(int i = 0; i < data.numStudents; i++){
		sum = sum + (float)data.students[i].grade;
	}
	stats.average = sum / (float)data.numStudents;
	return 0;
}

float popSDev(Dataset& data, Statistics& stats){
	float popDev = 0;
	for(int i = 0; i < data.numStudents; i++){
		popDev = pow(((float)data.students[i].grade - stats.average), 2) + popDev;
	}
	stats.popStdDev = sqrt(popDev / (float)(data.numStudents));
	return 0;
}

float smplSDev(Dataset& data, Statistics& stats){
	float smplDev = 0;
	for(int i = 0; i < data.numStudents; i++){
		smplDev = pow(((float)data.students[i].grade - stats.average), 2) + smplDev;
	}
	stats.smplStdDev = sqrt(smplDev / (float)(data.numStudents - 1));
	return 0;
}

int getMode(Dataset& data, Statistics& stats){
	int size = data.numStudents;
	int arr[size];
	for(int z = 0; z < size; z++){
		arr[z] = data.students[z].grade;
	}

	int i = 1;
	int modCount = 0;
	for (int j = 0; j < (size - 1); j++) {
		if (arr[j] == arr[j + 1]) {
			i++;
			if (i > modCount) {
				modCount = i;
			}
		}
		else
			i = 1;
	}

	if (modCount == 0){
		modCount = 1;
	}

	int k = 0;
	int modeCount = 0;
	i = 1;
	int out = 0;
	stats.mode.modes = new int [100];
	while(((out + modCount) <= (size))){
		for (int j = k; j < (size - 1); j++) {
			if (arr[k] == arr[j + 1])
				i++;
		}
		if (i == modCount) {
			stats.mode.modes[modeCount] = arr[k];
			modeCount++;
		}
		k = k + i;
		i = 1;
		out = k;
	}
	stats.mode.numModes = modeCount;
	return 0;
}

int main(const int argc, const char* const argv[]) {
  	//Testing Code
	int minAcceptableID = 0;
	int maxAcceptableID = 2000000;

	Dataset data = {0};
	Rejects rejects = {0};
	Statistics stats = {0};

	std::cout << std::endl << "Implement some more appropriate tests in main()" << std::endl << std::endl;

	if(readCSV(argv[1], minAcceptableID, maxAcceptableID, data, rejects) < 0)
	{
		std::cout << ".csv file could not be read" << std::endl;
	}

	if (computeStatistics(data, stats) < 0)
	{
		std::cout << "Stats could not be computed" << std::endl;
	}

	if (writeCSV(argv[1], stats) < 0)
	{
		std::cout << ".stat file could not be written" << std::endl;
	}

	std::cout << "Average: " << stats.average << std::endl;
	std::cout << "Minimum: " << stats.minimum << std::endl;
	std::cout << "Maximum: " << stats.maximum << std::endl;
	std::cout << "Population standard deviation: " << stats.popStdDev << std::endl;
	std::cout << "Sample standard deviation: " << stats.smplStdDev << std::endl;
	
	
	for (unsigned char i = 0; i < stats.mode.numModes; i++)
	{
		std::cout << "Mode: " << stats.mode.modes[i] << std::endl;
	}

	for (unsigned char i = 0; i < 10; i++)
	{
		std::cout << "Histogram bin [" << (i*10) << ", " << ((((i+1)*10) - 1) + i/9) << "]: " << stats.histogram[i] << std::endl;
	}
	

	return 0;
}