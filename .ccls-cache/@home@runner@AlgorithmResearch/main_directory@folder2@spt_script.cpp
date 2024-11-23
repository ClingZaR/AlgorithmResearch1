#include "spt_script.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
using namespace std;

void scheduleSPT(const vector<int>& tasks, int numMachines, ofstream& outputFile, ofstream& assignmentsFile, int classNumber, int instanceNumber) {
    vector<int> machineTimes(numMachines, 0);
    vector<int> sortedTasks = tasks;
    vector<int> taskAssignments(tasks.size());

    sort(sortedTasks.begin(), sortedTasks.end());

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < sortedTasks.size(); ++i) {
        int task = sortedTasks[i];
        int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
        machineTimes[minMachine] += task;
        taskAssignments[i] = minMachine + 1; 
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

void runSPT() {
    ifstream inputFile("main_directory/input.txt");
    ofstream outputFile("main_directory/output/spt_output.txt");
    ofstream assignmentsFile("main_directory/output/spt_assignments.txt");

    if (!inputFile || !outputFile || !assignmentsFile) {
        cerr << "Error opening files for SPT." << endl;
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

        scheduleSPT(tasks, numMachines, outputFile, assignmentsFile, classNumber, instanceNumber);
    }

    inputFile.close();
    outputFile.close();
    assignmentsFile.close();
}