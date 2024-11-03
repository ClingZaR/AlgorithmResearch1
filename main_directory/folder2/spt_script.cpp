#include "spt_script.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

void scheduleSPT(const vector<int>& tasks, int numMachines, ofstream& outputFile) {
    vector<int> machineTimes(numMachines, 0);
    vector<int> sortedTasks = tasks;

    sort(sortedTasks.begin(), sortedTasks.end());

    for (int task : sortedTasks) {
        int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
        machineTimes[minMachine] += task;
        // Uncomment the line below to show detailed task assignment
        // outputFile << "Assigned task " << task << " to machine " << minMachine + 1 << endl;
    }

    int Cmax = *max_element(machineTimes.begin(), machineTimes.end());
    int Cmin = *min_element(machineTimes.begin(), machineTimes.end());
    outputFile << "Cmin: " << Cmin << ", Cmax: " << Cmax << ", Difference: " << (Cmax - Cmin) << "\n" << endl;
}

void runSPT() {
    ifstream inputFile("main_directory/input.txt");
    ofstream outputFile("main_directory/output/spt_output.txt");

    if (!inputFile || !outputFile) {
        cerr << "Error opening files for SPT." << endl;
        return;
    }

    int numJobs, numMachines, classNumber, instanceNumber;

    while (inputFile >> numJobs >> numMachines >> classNumber >> instanceNumber) {
        vector<int> tasks(numJobs);
        for (int i = 0; i < numJobs; ++i) {
            inputFile >> tasks[i];
        }

        outputFile << "Class " << classNumber << ", Instance " << instanceNumber << ":\n";
        scheduleSPT(tasks, numMachines, outputFile);
    }

    inputFile.close();
    outputFile.close();
}
