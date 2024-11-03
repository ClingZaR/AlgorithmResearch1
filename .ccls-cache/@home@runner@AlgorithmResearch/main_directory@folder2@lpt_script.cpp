#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

void scheduleLPT(const vector<int>& tasks, int numMachines, ofstream& outputFile) {
    vector<int> machineTimes(numMachines, 0);
    vector<int> sortedTasks = tasks;

    sort(sortedTasks.rbegin(), sortedTasks.rend());  
    outputFile << "LPT Schedule:" << endl;

    for (int task : sortedTasks) {
        int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
        machineTimes[minMachine] += task;
        outputFile << "Assigned task " << task << " to machine " << minMachine + 1 << endl;
    }

    int Cmax = *max_element(machineTimes.begin(), machineTimes.end());
    outputFile << "Cmax: " << Cmax << "\n" << endl;
}

int main() {
    ifstream inputFile("../input.txt");
    ofstream outputFile("../output/lpt_output.txt");

    if (!inputFile || !outputFile) {
        cerr << "Error opening files." << endl;
        return 1;
    }

    int range, numTasks, numMachinesInput, classNumber, instanceNumber;
    inputFile >> range;

    while (inputFile >> numTasks >> numMachinesInput >> classNumber >> instanceNumber) {
        vector<int> tasks(numTasks);
        for (int& task : tasks) inputFile >> task;

        outputFile << "Class " << classNumber << ", Instance " << instanceNumber << ":\n";
        scheduleLPT(tasks, numMachinesInput, outputFile);
    }

    inputFile.close();
    outputFile.close();
    return 0;
}
