#include <iostream>
#include <fstream>
#include <random>
using namespace std;

void randomizeJobs(ofstream& outputFile, int classNum) {
    random_device rd;
    mt19937 gen(rd());

    int jobRangeStart = 1, jobRangeEnd = 100;
    switch (classNum) {
        case 2: jobRangeStart = 30; break;
        case 3: jobRangeStart = 50; break;
        case 4: jobRangeStart = 80; break;
        case 5: jobRangeStart = 80; jobRangeEnd = 300; break;
    }
    uniform_int_distribution<> dis(jobRangeStart, jobRangeEnd);

    int numJobs = dis(gen);
    outputFile << numJobs << endl;
}

int main() {
    ifstream inputFile("../input.txt");
    ofstream outputFile("../output/randomized_input.txt");

    if (!inputFile || !outputFile) {
        cerr << "Error opening files." << endl;
        return 1;
    }

    int range, numTasks, numMachinesInput, classNumber, instanceNumber;
    inputFile >> range;
    outputFile << range << endl;

    while (inputFile >> numTasks >> numMachinesInput >> classNumber >> instanceNumber) {
        outputFile << numMachinesInput << " " << classNumber << " " << instanceNumber << endl;
        randomizeJobs(outputFile, classNumber);
    }

    inputFile.close();
    outputFile.close();
    return 0;
}
