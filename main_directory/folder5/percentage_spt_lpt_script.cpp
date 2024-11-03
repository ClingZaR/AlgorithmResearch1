#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
using namespace std;

void schedulePercentageSPT_LPT(const vector<int>& tasks, int numMachines, int sptPercentage, int& bestPercentage, int& bestDifference) {
    vector<int> machineTimes(numMachines, 0);
    int sptCount = static_cast<int>(ceil((sptPercentage / 100.0) * tasks.size()));
    vector<int> firstPart(tasks.begin(), tasks.begin() + sptCount);
    vector<int> secondPart(tasks.begin() + sptCount, tasks.end());

    sort(firstPart.begin(), firstPart.end());      // SPT for the first part
    sort(secondPart.rbegin(), secondPart.rend());  // LPT for the second part

    // Assign SPT tasks
    for (int task : firstPart) {
        int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
        machineTimes[minMachine] += task;
    }

    // Assign LPT tasks
    for (int task : secondPart) {
        int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
        machineTimes[minMachine] += task;
    }

    int Cmax = *max_element(machineTimes.begin(), machineTimes.end());
    int Cmin = *min_element(machineTimes.begin(), machineTimes.end());
    int difference = Cmax - Cmin;

    // Update the best percentage if this one has a smaller difference
    if (difference < bestDifference) {
        bestDifference = difference;
        bestPercentage = sptPercentage;
    }
}

void runPercentageSPT_LPT() {
    ifstream inputFile("main_directory/input.txt");
    ofstream outputFile("main_directory/output/percentage_output.txt");

    if (!inputFile || !outputFile) {
        cerr << "Error opening files for Percentage SPT-LPT." << endl;
        return;
    }

    int numJobs, numMachines, classNumber, instanceNumber;

    while (inputFile >> numJobs >> numMachines >> classNumber >> instanceNumber) {
        vector<int> tasks(numJobs);
        for (int i = 0; i < numJobs; ++i) {
            inputFile >> tasks[i];
        }

        int bestPercentage = 0;
        int bestDifference = numeric_limits<int>::max();

        // Test each percentage and find the best one
        for (int sptPercentage = 5; sptPercentage <= 95; sptPercentage += 5) {
            schedulePercentageSPT_LPT(tasks, numMachines, sptPercentage, bestPercentage, bestDifference);
        }

        // Output the best result for this Class : Instance
        outputFile << "Class " << classNumber << ", Instance " << instanceNumber << ":\n";
        outputFile << "Best SPT " << bestPercentage << "%: Difference = " << bestDifference<< "\n" << endl;
    }

    inputFile.close();
    outputFile.close();
}
