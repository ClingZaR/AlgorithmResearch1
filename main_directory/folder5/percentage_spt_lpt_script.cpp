#include "percentage_spt_lpt_script.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>
using namespace std;

namespace fs = std::filesystem;

// Function to schedule tasks based on SPT and LPT percentage split
int schedulePercentageSPT_LPT(const vector<int> &tasks, int numMachines,
                               int sptPercentage, ofstream &summaryFile,
                               ofstream &assignmentsFile, int classNumber,
                               int instanceNumber) {
    vector<int> machineTimes(numMachines, 0);
    vector<int> taskAssignments(tasks.size());

    // Split the tasks based on the SPT percentage
    int sptCount = static_cast<int>(round((sptPercentage / 100.0) * tasks.size()));
    vector<int> firstPart(tasks.begin(), tasks.begin() + sptCount);
    vector<int> secondPart(tasks.begin() + sptCount, tasks.end());

    // Sort tasks: SPT for the first part and LPT for the second part
    sort(firstPart.begin(), firstPart.end());        // Shortest Processing Time
    sort(secondPart.rbegin(), secondPart.rend());    // Longest Processing Time

    auto start = chrono::high_resolution_clock::now();

    int taskIndex = 0;

    // Assign tasks from the SPT part
    for (int task : firstPart) {
        int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
        machineTimes[minMachine] += task;
        taskAssignments[taskIndex++] = minMachine + 1; // Assign task to machine (1-indexed)
    }

    // Assign tasks from the LPT part
    for (int task : secondPart) {
        int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
        machineTimes[minMachine] += task;
        taskAssignments[taskIndex++] = minMachine + 1; // Assign task to machine (1-indexed)
    }

    auto end = chrono::high_resolution_clock::now();

    // Calculate makespan (Cmax)
    int Cmax = *max_element(machineTimes.begin(), machineTimes.end());
    double timeTaken = chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9;

    // Write the summary
    summaryFile << tasks.size() << " " << numMachines << " " << classNumber << " "
                << instanceNumber << " " << Cmax << " " << fixed
                << setprecision(9) << timeTaken << endl << endl;

    // Write the task-to-machine assignments
    assignmentsFile << tasks.size() << " " << numMachines << " " << classNumber
                    << " " << instanceNumber << endl;
    for (int assignment : taskAssignments) {
        assignmentsFile << assignment << " ";
    }
    assignmentsFile << endl << endl; // Only write assignments, no tasks data.

    return Cmax; // Return the makespan (Cmax) for this instance
}

void runPercentageSPT_LPT() {
    ifstream inputFile("main_directory/input.txt");
    if (!inputFile) {
        cerr << "Error opening input file for Percentage SPT-LPT." << endl;
        return;
    }

    string outputDirectory = "main_directory/output/percentage_output";

    // Create the main directory for percentage outputs
    if (!fs::exists(outputDirectory)) {
        fs::create_directories(outputDirectory);
    }

    string firstLine;
    getline(inputFile, firstLine);
    int numJobs, numMachines, classNumber, instanceNumber;

    int bestPercentage = -1;
    int bestCmax = numeric_limits<int>::max();

    // Loop through percentages (5% to 95%)
    for (int sptPercentage = 5; sptPercentage <= 95; sptPercentage += 5) {
        string percentageFolder = outputDirectory + "/percentage_" + to_string(sptPercentage);
        if (!fs::exists(percentageFolder)) {
            fs::create_directories(percentageFolder);
        }

        string summaryFilePath = percentageFolder + "/summary_output.txt";
        string assignmentsFilePath = percentageFolder + "/assignments_output.txt";

        ofstream summaryFile(summaryFilePath, std::ios::trunc);
        ofstream assignmentsFile(assignmentsFilePath, std::ios::trunc);

        if (!summaryFile || !assignmentsFile) {
            cerr << "Error opening output files for percentage " << sptPercentage << "." << endl;
            continue;
        }

        // Reset input file to start reading from the second line again for each percentage
        inputFile.clear();
        inputFile.seekg(0, ios::beg);
        getline(inputFile, firstLine); // Skip the first line which is already read

        int totalCmax = 0;

        while (inputFile >> numJobs >> numMachines >> classNumber >> instanceNumber) {
            vector<int> tasks(numJobs);
            for (int i = 0; i < numJobs; ++i) {
                inputFile >> tasks[i];
            }

            // Run scheduling for the current percentage
            int Cmax = schedulePercentageSPT_LPT(tasks, numMachines, sptPercentage, summaryFile,
                                                 assignmentsFile, classNumber, instanceNumber);
            totalCmax += Cmax;
        }

        summaryFile.close();
        assignmentsFile.close();

        // cout << "Total Cmax for " << sptPercentage << "%: " << totalCmax << endl;

        // Update best percentage if the current totalCmax is lower
        if (totalCmax < bestCmax) {
            bestCmax = totalCmax;
            bestPercentage = sptPercentage;
        }
    }

    inputFile.close();

    // Write the best percentage and Cmax to the best_percentage.txt file
    string bestFilePath = outputDirectory + "/best_percentage.txt";
    ofstream bestFile(bestFilePath, std::ios::trunc);
    if (!bestFile) {
        cerr << "Error opening best_percentage.txt for writing." << endl;
        return;
    }

    bestFile << "Best Percentage: " << bestPercentage << "%" << endl;
    bestFile << "Cumulative Cmax: " << bestCmax << endl;
    bestFile.close();

    //cout << "Best Percentage for Percentage SPT-LPT: " << bestPercentage << "% with Cumulative Cmax = " << bestCmax << endl;
}

