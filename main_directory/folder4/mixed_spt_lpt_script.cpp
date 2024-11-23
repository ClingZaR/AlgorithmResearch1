#include "mixed_spt_lpt_script.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>
using namespace std;

void scheduleMixedSPTLPT(const vector<int>& tasks, int numMachines, ofstream& outputFile, ofstream& assignmentsFile, int classNumber, int instanceNumber) {
    vector<int> machineTimes(numMachines, 0);
    vector<int> sortedTasks = tasks;
    vector<int> taskAssignments(tasks.size());

    int midpoint = tasks.size() / 2;
    vector<int> firstHalf(tasks.begin(), tasks.begin() + midpoint);
    vector<int> secondHalf(tasks.begin() + midpoint, tasks.end());

    sort(firstHalf.begin(), firstHalf.end()); 
    sort(secondHalf.rbegin(), secondHalf.rend()); 

    auto start = chrono::high_resolution_clock::now();

    int taskIndex = 0;

    for (int task : firstHalf) {
        int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
        machineTimes[minMachine] += task;
        taskAssignments[taskIndex++] = minMachine + 1; 
    }

    for (int task : secondHalf) {
        int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
        machineTimes[minMachine] += task;
        taskAssignments[taskIndex++] = minMachine + 1; 
    }

    auto end = chrono::high_resolution_clock::now();

    int Cmax = *max_element(machineTimes.begin(), machineTimes.end());
    double timeTaken = chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9;

    outputFile << tasks.size() << " " << numMachines << " " << classNumber << " " << instanceNumber << " " << Cmax << " " << fixed << setprecision(9) << timeTaken << endl << endl;

    assignmentsFile << tasks.size() << " " << numMachines << " " << classNumber << " " << instanceNumber << endl;
    for (int i = 0; i < tasks.size(); ++i) {
        assignmentsFile << taskAssignments[i] << " ";
    }
    assignmentsFile << endl << endl;
}

void runMixedSPTLPT() {
    ifstream inputFile("main_directory/input.txt");
    ofstream outputFile("main_directory/output/mixed_spt_lpt_output.txt");
    ofstream assignmentsFile("main_directory/output/mixed_spt_lpt_assignments.txt");

    if (!inputFile || !outputFile || !assignmentsFile) {
        cerr << "Error opening files for Mixed SPT-LPT." << endl;
        return;
    }

    string firstLine;
    getline(inputFile, firstLine); 
    int numJobs, numMachines, classNumber, instanceNumber;

    while (inputFile >> numJobs >> numMachines >> classNumber >> instanceNumber) {
        vector<int> tasks(numJobs);
        for (int i = 0; i < numJobs; ++i) {
            inputFile >> tasks[i];
        }

        scheduleMixedSPTLPT(tasks, numMachines, outputFile, assignmentsFile, classNumber, instanceNumber);
    }

    inputFile.close();
    outputFile.close();
    assignmentsFile.close();
}