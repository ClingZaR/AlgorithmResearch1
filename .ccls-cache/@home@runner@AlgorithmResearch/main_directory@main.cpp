#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <sstream>
#include <limits>
#include "folder1/lpt_script.h"
#include "folder2/spt_script.h"
#include "folder3/mixed_lpt_spt_script.h"
#include "folder4/mixed_spt_lpt_script.h"
#include "folder5/percentage_spt_lpt_script.h"
using namespace std;

void generateMappedInputFile(const string &fileName, int instancesPerPair) {
    ofstream inputFile(fileName);
    if (!inputFile) {
        cerr << "Failed to create input file: " << fileName << endl;
        exit(1);
    }

    random_device rd;
    mt19937 gen(rd());

    // Define the mapping: jobs (n) to machines (m)
    vector<pair<vector<int>, vector<int>>> mappings = {
        {{5, 6, 7}, {2, 3}},        // For jobs 5, 6, 7 -> machines 2, 3
        {{10, 12, 15}, {3, 5, 7}}   // For jobs 10, 12, 15 -> machines 3, 5, 7
    };

    // Define ranges of processing times for each class
    vector<pair<int, int>> classRanges = {{1, 100}, {30, 100}, {50, 100}, {80, 100}, {80, 300}};

    // Count total instances
    int totalInstances = 0;
    for (const auto &mapping : mappings) {
        totalInstances += mapping.first.size() * mapping.second.size() * instancesPerPair;
    }

    // Write the total number of instances
    inputFile << totalInstances << endl << endl;

    // Generate instances for each mapping
    int classNumber = 1;
    for (const auto &mapping : mappings) {
        const vector<int> &jobCounts = mapping.first;
        const vector<int> &machineCounts = mapping.second;

        for (int n : jobCounts) {
            for (int m : machineCounts) {
                // Fetch processing time range for the current class
                int minLoad = classRanges[(classNumber - 1) % classRanges.size()].first;
                int maxLoad = classRanges[(classNumber - 1) % classRanges.size()].second;
                uniform_int_distribution<> dis(minLoad, maxLoad);

                for (int instance = 1; instance <= instancesPerPair; ++instance) {
                    // Write instance metadata
                    inputFile << n << " " << m << " " << classNumber << " " << instance << endl;

                    // Generate random processing times for jobs
                    for (int i = 0; i < n; ++i) {
                        inputFile << dis(gen);
                        if (i != n - 1) inputFile << " ";
                    }
                    inputFile << endl << endl;
                }
                ++classNumber; // Increment class number for each n-m combination
            }
        }
    }

    inputFile.close();
    cout << "Input file successfully generated: " << fileName << endl;
}

void findBestAlgorithm() {
    ifstream inputFile("main_directory/input.txt");
    if (!inputFile) {
        cerr << "Failed to open main_directory/input.txt for reading metadata." << endl;
        return;
    }

    int totalInstances;
    inputFile >> totalInstances;
    inputFile.close();

    vector<string> files = {
        "main_directory/output/lpt_output.txt",
        "main_directory/output/spt_output.txt",
        "main_directory/output/mixed_lpt_spt_output.txt",
        "main_directory/output/mixed_spt_lpt_output.txt",
        "main_directory/output/percentage_output/summary_output.txt"
    };
    vector<string> algorithmNames = {"LPT", "SPT", "50% LPT-SPT", "50% SPT-LPT", "Percentage SPT-LPT"};

    vector<int> cumulativeCmax(files.size(), 0);

    int bestPercentage = -1;
    int bestPercentageCmax = numeric_limits<int>::max();

    for (size_t i = 0; i < files.size(); ++i) {
        ifstream file(files[i]);
        if (!file) {
            cerr << "Failed to open " << files[i] << endl;
            continue;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            int numJobs, numMachines, currClass, currInstance, Cmax;
            double timeTaken;
            istringstream iss(line);
            iss >> numJobs >> numMachines >> currClass >> currInstance >> Cmax >> timeTaken;

            cumulativeCmax[i] += Cmax;

            // For Percentage SPT-LPT, determine the best percentage
            if (i == 4) { // Percentage SPT-LPT algorithm
                int percentage;
                iss >> percentage; // Extract the percentage column
                if (Cmax < bestPercentageCmax) {
                    bestPercentageCmax = Cmax;
                    bestPercentage = percentage;
                }
            }
        }
        file.close();
    }

    int bestAlgorithmIndex = 0;
    int leastCmax = cumulativeCmax[0];
    for (size_t i = 1; i < cumulativeCmax.size(); ++i) {
        if (cumulativeCmax[i] < leastCmax) {
            leastCmax = cumulativeCmax[i];
            bestAlgorithmIndex = i;
        }
    }

    cout << "Cumulative Cmax for each algorithm:" << endl;
    for (size_t i = 0; i < algorithmNames.size(); ++i) {
        cout << algorithmNames[i] << ": " << cumulativeCmax[i] << endl;
    }
    cout << "Best Algorithm: " << algorithmNames[bestAlgorithmIndex]
         << " with Cumulative Cmax = " << leastCmax << endl;

    if (bestAlgorithmIndex == 4) { // If Percentage SPT-LPT is the best
        cout << "Best Percentage for Percentage SPT-LPT: " << bestPercentage
             << "% with Cmax = " << bestPercentageCmax << endl;
    }
}

int main() {
    string fileName = "main_directory/input.txt";
    int instancesPerPair = 10; // Number of instances for each (n, m) pair

    // Generate input file with n-m mapping and class-based processing time ranges
    generateMappedInputFile(fileName, instancesPerPair);

    // Run algorithms
    runLPT();
    runSPT();
    runMixedLPTSPT();
    runMixedSPTLPT();
    runPercentageSPT_LPT();

    // Find the best algorithm
    findBestAlgorithm();

    return 0;
}
