#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
#include <cmath>
#include <random>
#include "folder1/lpt_script.h"
#include "folder2/spt_script.h"
#include "folder3/mixed_lpt_spt_script.h"
#include "folder4/mixed_spt_lpt_script.h"
#include "folder5/percentage_spt_lpt_script.h"
using namespace std;

// Function to parse the difference from a line in the output file
int parseDifference(const string& line) {
    size_t pos = line.find("Difference: ");
    if (pos != string::npos) {
        // For non-percentage-based lines
        int difference = stoi(line.substr(pos + 11));
       // cout << "Parsed Difference (non-percentage): " << difference << " from line: " << line << endl;
        return difference;
    }

    pos = line.find("Best SPT ");
    if (pos != string::npos) {
        // For percentage-based lines
        size_t diffPos = line.find("Difference = ");
        if (diffPos != string::npos) {
            int difference = stoi(line.substr(diffPos + 13));
        //    cout << "Parsed Difference (percentage): " << difference << " from line: " << line << endl;
            return difference;
        }
    }

    cout << "Failed to parse Difference from line: " << line << endl;
    return numeric_limits<int>::max();
}



void generateInputFile(int numJobs, int numMachines, int numClasses, int instancesPerClass) {
    ofstream inputFile("main_directory/input.txt");
    if (!inputFile) {
        cerr << "Failed to create main_directory/input.txt" << endl;
        exit(1);
    }

    random_device rd;
    mt19937 gen(rd());
    vector<pair<int, int>> classRanges = {{1, 100}, {30, 100}, {50, 100}, {80, 100}, {80, 300}};

    int totalInstances = numClasses * instancesPerClass; // Explicit total instances calculation

    inputFile << totalInstances << endl << endl;

    for (int classNum = 1; classNum <= numClasses; ++classNum) {
        int minLoad = classRanges[(classNum - 1) % classRanges.size()].first;
        int maxLoad = classRanges[(classNum - 1) % classRanges.size()].second;
        uniform_int_distribution<> dis(minLoad, maxLoad);

        for (int instance = 1; instance <= instancesPerClass; ++instance) {
            inputFile << numJobs << " " << numMachines << " " << classNum << " " << instance << endl;
            for (int job = 0; job < numJobs; ++job) {
                inputFile << dis(gen) << (job != numJobs - 1 ? " " : "\n");
            }
            inputFile << endl; // Add an empty line to separate instances
        }
    }
    inputFile.close();
}



void findBestAlgorithm() {
    ifstream inputFile("main_directory/input.txt");
    if (!inputFile) {
        cerr << "Failed to open main_directory/input.txt for reading metadata." << endl;
        return;
    }

    // Read the total number of instances from the first line
    int totalInstances;
    inputFile >> totalInstances;
    inputFile.close();

    // Predefined or inferred values
    int instancesPerClass = 10; // Known predefined parameter
    int numClasses = (totalInstances + instancesPerClass - 1) / instancesPerClass; // Calculate dynamically

    vector<string> files = {
        "main_directory/output/lpt_output.txt",
        "main_directory/output/spt_output.txt",
        "main_directory/output/mixed_lpt_spt_output.txt",
        "main_directory/output/mixed_spt_lpt_output.txt",
        "main_directory/output/percentage_output.txt"
    };
    vector<string> algorithmNames = {"LPT", "SPT", "50% LPT-SPT", "50% SPT-LPT", "Percentage SPT-LPT"};
    string line;

    for (int classNum = 1; classNum <= numClasses; ++classNum) {
        for (int instanceNum = 1; instanceNum <= instancesPerClass; ++instanceNum) {
            if ((classNum - 1) * instancesPerClass + instanceNum > totalInstances) break;

            int bestNonPercentageDifference = numeric_limits<int>::max();
            string bestNonPercentageAlgorithm = "";

            cout << "\nClass " << classNum << ", Instance " << instanceNum << ":\n";
            for (size_t i = 0; i < 4; ++i) {
                ifstream file(files[i]);
                if (!file) {
                    cerr << "Failed to open " << files[i] << endl;
                    continue;
                }

                bool foundInstance = false;
                string line;
                while (getline(file, line)) {
                    if (line == "Class " + to_string(classNum) + ", Instance " + to_string(instanceNum) + ":") {
                        foundInstance = true;

                        getline(file, line); // The line with Cmin, Cmax, and Difference
                        int difference = parseDifference(line);

                        if (difference < bestNonPercentageDifference) {
                            bestNonPercentageDifference = difference;
                            bestNonPercentageAlgorithm = algorithmNames[i];
                        }
                        break;
                    }
                }
                file.close();

                if (!foundInstance) {
                    cerr << "Error: Could not find Class " << classNum << ", Instance " << instanceNum << " in " << files[i] << endl;
                }
            }

            int bestPercentageDifference = numeric_limits<int>::max();
            int bestPercentageSPT = 0;
            ifstream percentageFile(files[4]);
            if (!percentageFile) {
                cerr << "Failed to open " << files[4] << endl;
                continue;
            }

            bool foundPercentageInstance = false;
            while (getline(percentageFile, line)) {
                if (line == "Class " + to_string(classNum) + ", Instance " + to_string(instanceNum) + ":") {
                    foundPercentageInstance = true;

                    // Process each percentage-based line for Percentage SPT-LPT
                    while (getline(percentageFile, line) && line.find("SPT") != string::npos) {
                        size_t pos = line.find("SPT ");
                        int currentPercentage = stoi(line.substr(pos + 4, line.find('%') - pos - 4));
                        int difference = parseDifference(line);

                        if (difference < bestPercentageDifference) {
                            bestPercentageDifference = difference;
                            bestPercentageSPT = currentPercentage;
                        }
                    }
                    break;
                }
            }

            if (!foundPercentageInstance) {
                cerr << "Error: Could not find Class " << classNum << ", Instance " << instanceNum << " in " << files[4] << endl;
            }

            percentageFile.close();

            string finalAlgorithm = bestNonPercentageAlgorithm;
            int finalDifference = bestNonPercentageDifference;
            int finalPercentage = 0;

            if (bestPercentageDifference < bestNonPercentageDifference) {
                finalAlgorithm = "Percentage SPT-LPT";
                finalDifference = bestPercentageDifference;
                finalPercentage = bestPercentageSPT;
            }

            cout << "Best Algorithm : " << finalAlgorithm;
            if (finalAlgorithm == "Percentage SPT-LPT") {
                cout << " with " << finalPercentage << "% SPT";
            }
            cout << ", Difference = " << finalDifference << endl;
        }
    }
}








// Change numClasses to 65 if you want 650 instances.

int main() {
    // Parameters for generating input
    int numJobs = 10, numMachines = 2, numClasses = 65, instancesPerClass = 10;

    // Step 1: Generate the input file
    generateInputFile(numJobs, numMachines, numClasses, instancesPerClass);

    // Step 2: Run each algorithm to produce output files
    runLPT();
    runSPT();
    runMixedLPTSPT();
    runMixedSPTLPT();
    runPercentageSPT_LPT();

    // Step 3: Find the best algorithm for each Class : Instance
    findBestAlgorithm();

    return 0;
}
