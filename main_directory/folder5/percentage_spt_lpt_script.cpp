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
#include <map>
using namespace std;

namespace fs = std::filesystem;

int schedulePercentageSPT_LPT(const vector<int> &tasks, int numMachines,
                               int sptPercentage, ofstream &summaryFile,
                               ofstream &assignmentsFile, int classNumber,
                               int instanceNumber) {
    vector<int> machineTimes(numMachines, 0);
    vector<int> taskAssignments(tasks.size());

    int sptCount = static_cast<int>(round((sptPercentage / 100.0) * tasks.size()));
    vector<int> firstPart(tasks.begin(), tasks.begin() + sptCount);
    vector<int> secondPart(tasks.begin() + sptCount, tasks.end());

    sort(firstPart.begin(), firstPart.end());        
    sort(secondPart.rbegin(), secondPart.rend());    

    auto start = chrono::high_resolution_clock::now();

    int taskIndex = 0;

    for (int task : firstPart) {
        int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
        machineTimes[minMachine] += task;
        taskAssignments[taskIndex++] = minMachine + 1; 
    }

    for (int task : secondPart) {
        int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
        machineTimes[minMachine] += task;
        taskAssignments[taskIndex++] = minMachine + 1; 
    }

    auto end = chrono::high_resolution_clock::now();

    int Cmax = *max_element(machineTimes.begin(), machineTimes.end());
    double timeTaken = chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9;

    summaryFile << tasks.size() << " " << numMachines << " " << classNumber << " "
                << instanceNumber << " " << Cmax << " " << fixed
                << setprecision(9) << timeTaken << endl << endl;

    assignmentsFile << tasks.size() << " " << numMachines << " " << classNumber
                    << " " << instanceNumber << endl;
    for (int assignment : taskAssignments) {
        assignmentsFile << assignment << " ";
    }
    assignmentsFile << endl << endl; 

    return Cmax; 
}

void runPercentageSPT_LPT() {
    ifstream inputFile("main_directory/input.txt");
    if (!inputFile) {
        cerr << "Error opening input file for Percentage SPT-LPT." << endl;
        return;
    }

    string outputDirectory = "main_directory/output/percentage_output";
    if (!fs::exists(outputDirectory)) {
        fs::create_directories(outputDirectory);
    }

    string firstLine;
    getline(inputFile, firstLine);

    int numJobs, numMachines, classNumber, instanceNumber;

    map<pair<int, int>, vector<int>> results;
    vector<int> percentages;

    for (int sptPercentage = 5; sptPercentage <= 95; sptPercentage += 5) {
        percentages.push_back(sptPercentage);

        string percentageFolder = outputDirectory + "/percentage_" + to_string(sptPercentage);
        if (!fs::exists(percentageFolder)) {
            fs::create_directories(percentageFolder);
        }

        string summaryFilePath = percentageFolder + "/summary_output.txt";
        string assignmentsFilePath = percentageFolder + "/assignments_output.txt";

        ofstream summaryFile(summaryFilePath, ios::trunc);
        ofstream assignmentsFile(assignmentsFilePath, ios::trunc);

        if (!summaryFile || !assignmentsFile) {
            cerr << "Error opening output files for percentage " << sptPercentage << "." << endl;
            continue;
        }

        inputFile.clear();
        inputFile.seekg(0, ios::beg);
        getline(inputFile, firstLine); 

        while (inputFile >> numJobs >> numMachines >> classNumber >> instanceNumber) {
            vector<int> tasks(numJobs);
            for (int i = 0; i < numJobs; ++i) {
                inputFile >> tasks[i];
            }

            int Cmax = schedulePercentageSPT_LPT(tasks, numMachines, sptPercentage,
                                                 summaryFile, assignmentsFile,
                                                 classNumber, instanceNumber);

            results[make_pair(numJobs * 100 + numMachines, classNumber * 10 + instanceNumber)].push_back(Cmax);
        }

        summaryFile.close();
        assignmentsFile.close();
    }

    inputFile.close();

    string csvFilePath = outputDirectory + "/percentage_spt_lpt_results.csv";
    ofstream csvFile(csvFilePath);
    if (!csvFile.is_open()) {
        cerr << "Error opening output CSV file." << endl;
        return;
    }

    csvFile << "Instance";
    for (int sptPercentage : percentages) {
        csvFile << "," << sptPercentage;
    }
    csvFile << ",,Min";
    for (int sptPercentage : percentages) {
        csvFile << ",Gap" << sptPercentage;
    }
    csvFile << ",Best %" << endl;

    vector<int> zeroCounts(percentages.size(), 0);

    for (const auto &entry : results) {
        const auto &instance = entry.first;
        const auto &cmaxValues = entry.second;

        int numJobs = instance.first / 100;
        int numMachines = instance.first % 100;
        int classNumber = instance.second / 10;
        int instanceNumber = instance.second % 10;

        csvFile << numJobs << " " << numMachines << " " << classNumber << " " << instanceNumber;

        int minCmax = *min_element(cmaxValues.begin(), cmaxValues.end());
        for (int cmax : cmaxValues) {
            csvFile << "," << cmax;
        }

        csvFile << ",," << minCmax;

        for (size_t i = 0; i < cmaxValues.size(); ++i) {
            double gap = (double)(cmaxValues[i] - minCmax) / minCmax;
            csvFile << "," << fixed << setprecision(5) << gap;
            if (gap == 0.0) {
                zeroCounts[i]++;
            }
        }

        csvFile << endl;
    }

    csvFile << "0 Count,,,,,,,,,,,,,,,,,,,,,";
    for (int count : zeroCounts) {
        csvFile << "," << count;
    }

    int bestPercentageIndex = max_element(zeroCounts.begin(), zeroCounts.end()) - zeroCounts.begin();
    int bestPercentage = percentages[bestPercentageIndex];

    csvFile << "," << bestPercentage << endl;

    csvFile.close();
    cout << "Results written to " << csvFilePath << endl;
}
