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
  int sptCount = static_cast<int>(ceil((sptPercentage / 100.0) * tasks.size()));
  vector<int> firstPart(tasks.begin(), tasks.begin() + sptCount);
  vector<int> secondPart(tasks.begin() + sptCount, tasks.end());

  // Sort tasks: SPT for the first part and LPT for the second part
  sort(firstPart.begin(), firstPart.end());
  sort(secondPart.rbegin(), secondPart.rend());

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
              << setprecision(9) << timeTaken << endl;

  // Write the task-to-machine assignments
  assignmentsFile << tasks.size() << " " << numMachines << " " << classNumber
                  << " " << instanceNumber << endl;
  for (int assignment : taskAssignments) {
    assignmentsFile << assignment << " ";
  }
  assignmentsFile << endl << endl;

  return Cmax; // Return the makespan (Cmax) for this instance
}


// Function to run the Percentage SPT-LPT algorithm
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

  // Store cumulative Cmax for each percentage (5% to 95%)
  vector<int> totalCmax(19, 0);

  while (inputFile >> numJobs >> numMachines >> classNumber >> instanceNumber) {
    vector<int> tasks(numJobs);
    for (int i = 0; i < numJobs; ++i) {
      inputFile >> tasks[i];
    }

    // Loop through percentages (5% to 95%)
      // Loop through percentages (5% to 95%)
      for (int step = 1, sptPercentage = 5; sptPercentage <= 95; ++step, sptPercentage += 5) {
        string percentageFolder = outputDirectory + "/percentage_" + to_string(sptPercentage);
        if (!fs::exists(percentageFolder)) {
          fs::create_directories(percentageFolder);
        }

        string summaryFilePath = percentageFolder + "/summary_output.txt";
        string assignmentsFilePath = percentageFolder + "/assignments_output.txt";

        // Open files for the current percentage
        ofstream summaryFile(summaryFilePath);
        ofstream assignmentsFile(assignmentsFilePath);

        if (!summaryFile || !assignmentsFile) {
          cerr << "Error opening output files for percentage " << sptPercentage << "." << endl;
          continue;
        }

        // Run scheduling for the current percentage and update cumulative Cmax
        int Cmax = schedulePercentageSPT_LPT(tasks, numMachines, sptPercentage, summaryFile,
                                             assignmentsFile, classNumber, instanceNumber);
        totalCmax[step - 1] += Cmax;

        summaryFile.close();
        assignmentsFile.close();
      }
  }

  inputFile.close();

  // Determine the best percentage based on cumulative Cmax
  int bestPercentage = 5;
  int minCmax = totalCmax[0];
  for (int i = 1, sptPercentage = 10; sptPercentage <= 95; ++i, sptPercentage += 5) {
    if (totalCmax[i] < minCmax) {
      minCmax = totalCmax[i];
      bestPercentage = sptPercentage;
    }
  }

  // Output cumulative Cmax for debugging
  cout << "Cumulative Cmax for each percentage:" << endl;
  for (int step = 1, sptPercentage = 5; sptPercentage <= 95; ++step, sptPercentage += 5) {
    cout << sptPercentage << "%: " << totalCmax[step - 1] << endl;
  }

  // Output the best percentage
  string bestPercentageFile = outputDirectory + "/best_percentage.txt";
  ofstream bestPercentageOutput(bestPercentageFile);
  if (bestPercentageOutput) {
    bestPercentageOutput << "Best percentage: " << bestPercentage << "%" << endl;
    bestPercentageOutput << "Cumulative Cmax: " << minCmax << endl;

    cout << "Best percentage: " << bestPercentage << "%" << endl;
    cout << "Cumulative Cmax for best percentage: " << minCmax << endl;
  } else {
    cerr << "Error writing best percentage output file." << endl;
  }
}
