#include "folder1/lpt_script.h"
#include "folder2/spt_script.h"
#include "folder3/mixed_lpt_spt_script.h"
#include "folder4/mixed_spt_lpt_script.h"
#include "folder5/percentage_spt_lpt_script.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <filesystem>
#include <random>
using namespace std;

namespace fs = std::filesystem;

void generateMappedInputFile(const string &fileName, int instancesPerClass) {
    ofstream inputFile(fileName);
    if (!inputFile) {
        cerr << "Failed to create input file: " << fileName << endl;
        exit(1);
    }

    random_device rd;
    mt19937 gen(rd());

    vector<pair<vector<int>, vector<int>>> mappings = {
        {{5, 6, 7}, {2, 3}},
        {{10, 12, 15}, {3, 5, 7}}
    };

    vector<pair<int, int>> classRanges = {
        {1, 100}, {30, 100}, {50, 100}, {80, 100}, {80, 300}
    };

    int totalInstances = 0;
    stringstream outputBuffer;

    for (const auto &mapping : mappings) {
        const vector<int> &jobCounts = mapping.first;
        const vector<int> &machineCounts = mapping.second;

        for (int n : jobCounts) {
            for (int m : machineCounts) {
                for (int classNumber = 1; classNumber <= 5; ++classNumber) {
                    int minLoad = classRanges[(classNumber - 1) % classRanges.size()].first;
                    int maxLoad = classRanges[(classNumber - 1) % classRanges.size()].second;
                    uniform_int_distribution<> dis(minLoad, maxLoad);

                    for (int instance = 1; instance <= instancesPerClass; ++instance) {
                        totalInstances++;
                        outputBuffer << n << " " << m << " " << classNumber << " " << instance << endl;

                        for (int i = 0; i < n; ++i) {
                            outputBuffer << dis(gen);
                            if (i != n - 1) outputBuffer << " ";
                        }
                        outputBuffer << endl << endl;
                    }
                }
            }
        }
    }

    inputFile << totalInstances << endl << endl;
    inputFile << outputBuffer.str();
    inputFile.close();
}

void runAlgorithmsAndGenerateCSV() {
    vector<string> algorithmFiles = {
        "main_directory/output/lpt_output.txt",
        "main_directory/output/spt_output.txt",
        "main_directory/output/mixed_lpt_spt_output.txt",
        "main_directory/output/mixed_spt_lpt_output.txt"
    };

    vector<string> algorithmNames = {"LPT", "SPT", "50% LPT-SPT", "50% SPT-LPT"};
    map<pair<int, int>, vector<int>> results;
    vector<int> cumulativeCmax(algorithmFiles.size(), 0);

    ifstream inputFile;
    for (size_t i = 0; i < algorithmFiles.size(); ++i) {
        inputFile.open(algorithmFiles[i]);
        if (!inputFile.is_open()) {
            cerr << "Error opening file: " << algorithmFiles[i] << endl;
            continue;
        }

        string line;
        while (getline(inputFile, line)) {
            if (line.empty() || line.find(':') != string::npos) continue;

            istringstream iss(line);
            int numJobs, numMachines, classNumber, instanceNumber, Cmax;
            double timeTaken;

            if (iss >> numJobs >> numMachines >> classNumber >> instanceNumber >> Cmax >> timeTaken) {
                results[{numJobs * 100 + numMachines, classNumber * 10 + instanceNumber}].push_back(Cmax);
                cumulativeCmax[i] += Cmax;
            }
        }
        inputFile.close();
    }

    string outputDirectory = "main_directory/output";
    string csvFilePath = outputDirectory + "/algorithm_comparison_results.csv";

    if (!fs::exists(outputDirectory)) {
        fs::create_directories(outputDirectory);
    }

    ofstream csvFile(csvFilePath);
    if (!csvFile.is_open()) {
        cerr << "Error opening output CSV file." << endl;
        return;
    }

    csvFile << "Instance";
    for (const string &algo : algorithmNames) {
        csvFile << "," << algo;
    }
    csvFile << ",,Min";
    for (const string &algo : algorithmNames) {
        csvFile << ",Gap " << algo;
    }
    csvFile << ",Best Algo ";
    csvFile << endl;

    vector<int> zeroCounts(algorithmNames.size(), 0);

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
            double gap = static_cast<double>(cmaxValues[i] - minCmax) / minCmax;
            csvFile << "," << fixed << setprecision(5) << gap;
            if (gap == 0.0) {
                zeroCounts[i]++;
            }
        }

        csvFile << endl;
    }

    csvFile << "0 Count,,,,,,";
    for (int count : zeroCounts) {
        csvFile << "," << count;
    }

    int bestAlgorithmIndex = max_element(zeroCounts.begin(), zeroCounts.end()) - zeroCounts.begin();
    string bestAlgorithm = algorithmNames[bestAlgorithmIndex];

    csvFile << "," << bestAlgorithm << endl;

    csvFile.close();
    cout << "Results written to " << csvFilePath << endl;

    cout << "Cumulative Cmax for each algorithm:" << endl;
    for (size_t i = 0; i < algorithmFiles.size(); ++i) {
        cout << algorithmNames[i] << ": " << cumulativeCmax[i] << endl;
    }

    string percentageFile = "main_directory/output/percentage_output/best_percentage.txt";
    ifstream percentageInput(percentageFile);
    if (percentageInput.is_open()) {
        string line;
        int bestPercentageCmax = 0;

        while (getline(percentageInput, line)) {
            if (line.find("Cumulative Cmax:") != string::npos) {
                bestPercentageCmax = stoi(line.substr(line.find(":") + 1));
            }
        }
        cout << "Percentage SPT-LPT: " << bestPercentageCmax << endl;
        percentageInput.close();
    } else {
        cerr << "Error reading Percentage SPT-LPT results." << endl;
    }

    cout << "Best Algorithm: " << bestAlgorithm << endl;
}

int main() {
    string fileName = "main_directory/input.txt";
    int instancesPerClass = 10;

    generateMappedInputFile(fileName, instancesPerClass);

    runLPT();
    runSPT();
    runMixedLPTSPT();
    runMixedSPTLPT();
    runPercentageSPT_LPT();

    runAlgorithmsAndGenerateCSV();

    return 0;
}
