#include "folder1/lpt_script.h"
#include "folder2/spt_script.h"
#include "folder3/mixed_lpt_spt_script.h"
#include "folder4/mixed_spt_lpt_script.h"
#include "folder5/percentage_spt_lpt_script.h"
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

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
    stringstream outputBuffer; // Buffer to store the output temporarily

    for (const auto &mapping : mappings) {
        const vector<int> &jobCounts = mapping.first;
        const vector<int> &machineCounts = mapping.second;

        for (int n : jobCounts) { // Iterate over job counts first
            for (int m : machineCounts) { // Iterate over machine counts
                // Generate 5 classes for this combination of n and m
                for (int classNumber = 1; classNumber <= 5; ++classNumber) {
                    int minLoad = classRanges[(classNumber - 1) % classRanges.size()].first;
                    int maxLoad = classRanges[(classNumber - 1) % classRanges.size()].second;
                    uniform_int_distribution<> dis(minLoad, maxLoad);

                    for (int instance = 1; instance <= instancesPerClass; ++instance) {
                        totalInstances++;
                        outputBuffer << n << " " << m << " " << classNumber << " " << instance << endl;

                        // Generate loads for `n` jobs
                        for (int i = 0; i < n; ++i) {
                            outputBuffer << dis(gen);
                            if (i != n - 1)
                                outputBuffer << " ";
                        }
                        outputBuffer << endl << endl;
                    }
                }
            }
        }
    }

    // Write the total number of instances at the start of the file
    inputFile << totalInstances << endl << endl;

    // Write the buffered output
    inputFile << outputBuffer.str();

    inputFile.close();
    // cout << "Input file successfully generated: " << fileName << endl;
}






void findBestAlgorithm() {
  vector<string> files = {
      "main_directory/output/lpt_output.txt",
      "main_directory/output/spt_output.txt",
      "main_directory/output/mixed_lpt_spt_output.txt",
      "main_directory/output/mixed_spt_lpt_output.txt",
      "main_directory/output/percentage_output/best_percentage.txt"};

  vector<string> algorithmNames = {"LPT", "SPT", "50% LPT-SPT", "50% SPT-LPT",
                                   "Percentage SPT-LPT"};

  vector<int> cumulativeCmax(files.size(), 0);
  int bestPercentage = -1;
  int bestPercentageCmax = numeric_limits<int>::max();

  for (size_t i = 0; i < files.size(); ++i) {
    // cout << "Processing algorithm index: " << i << " (" << algorithmNames[i]
    // << ")" << endl;

    ifstream file(files[i]);
    if (!file) {
      cerr << "Failed to open " << files[i] << " for reading." << endl;
      continue;
    }

    if (i == 4) { // Special case for percentage output
     //  cout << "Processing percentage output file: " << files[i] << endl;

      string bestPercentageLine, bestCmaxLine;
      if (getline(file, bestPercentageLine) && getline(file, bestCmaxLine)) {
        //   cout << "Reading best_percentage.txt..." << endl;
        //  cout << "Line 1: " << bestPercentageLine << endl;
        //   cout << "Line 2: " << bestCmaxLine << endl;

        try {
          size_t percentPos = bestPercentageLine.find('%');
          if (percentPos != string::npos) {
            size_t colonPos = bestPercentageLine.find(':');
            bestPercentage = stoi(bestPercentageLine.substr(
                colonPos + 1, percentPos - colonPos - 1));
          //  cout << "Parsed Best Percentage: " << bestPercentage << "%" << endl;
          } else {
            cerr << "Percentage line format not as expected: "
                 << bestPercentageLine << endl;
          }

          size_t colonPos = bestCmaxLine.find(':');
          if (colonPos != string::npos) {
            bestPercentageCmax = stoi(bestCmaxLine.substr(colonPos + 1));
            cumulativeCmax[i] = bestPercentageCmax;
           // cout << "Parsed Cmax: " << bestPercentageCmax << endl;
          } else {
            cerr << "Cmax line format not as expected: " << bestCmaxLine
                 << endl;
          }
        } catch (const exception &e) {
          cerr << "Error parsing percentage or Cmax: " << e.what() << endl;
        }
      } else {
        cerr << "Failed to read lines from best_percentage.txt" << endl;
      }

      continue;
    }

    string line;
    while (getline(file, line)) {
      if (line.empty() || line.find(':') != string::npos)
        continue;

      istringstream iss(line);
      int numJobs, numMachines, currClass, currInstance, Cmax;
      double timeTaken;
      if (iss >> numJobs >> numMachines >> currClass >> currInstance >> Cmax >>
          timeTaken) {
        cumulativeCmax[i] += Cmax;
      } else {
        cerr << "Error parsing line: " << line << endl;
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

  if (bestAlgorithmIndex == 4) {
    cout << "Best Percentage for Percentage SPT-LPT: " << bestPercentage
         << "%"<< endl;
  }
}

int main() {
  string fileName = "main_directory/input.txt";
  int instancesPerPair = 10;

  generateMappedInputFile(fileName, instancesPerPair);

  runLPT();
  runSPT();
  runMixedLPTSPT();
  runMixedSPTLPT();
  runPercentageSPT_LPT();

  findBestAlgorithm();

  return 0;
}
