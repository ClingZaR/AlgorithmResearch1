    #include "mixed_spt_lpt_script.h"
    #include <iostream>
    #include <fstream>
    #include <vector>
    #include <algorithm>
    using namespace std;

    void mixedSPT_LPT(const vector<int>& tasks, int numMachines, ofstream& outputFile) {
        vector<int> machineTimes(numMachines, 0);
        int midpoint = tasks.size() / 2;
        vector<int> firstHalf(tasks.begin(), tasks.begin() + midpoint);
        vector<int> secondHalf(tasks.begin() + midpoint, tasks.end());

        sort(firstHalf.begin(), firstHalf.end());
        sort(secondHalf.rbegin(), secondHalf.rend());

        for (int task : firstHalf) {
            int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
            machineTimes[minMachine] += task;
            // Uncomment the line below to show detailed task assignment
            // outputFile << "Assigned task " << task << " to machine " << minMachine + 1 << " (SPT)" << endl;
        }
        for (int task : secondHalf) {
            int minMachine = min_element(machineTimes.begin(), machineTimes.end()) - machineTimes.begin();
            machineTimes[minMachine] += task;
            // Uncomment the line below to show detailed task assignment
            // outputFile << "Assigned task " << task << " to machine " << minMachine + 1 << " (LPT)" << endl;
        }

        int Cmax = *max_element(machineTimes.begin(), machineTimes.end());
        int Cmin = *min_element(machineTimes.begin(), machineTimes.end());
        outputFile << "Cmin: " << Cmin << ", Cmax: " << Cmax << ", Difference: " << (Cmax - Cmin) << "\n" << endl;
    }

    void runMixedSPTLPT() {
        ifstream inputFile("main_directory/input.txt");
        ofstream outputFile("main_directory/output/mixed_spt_lpt_output.txt");

        if (!inputFile || !outputFile) {
            cerr << "Error opening files for Mixed SPT-LPT." << endl;
            return;
        }
        string firstLine;
        getline(inputFile, firstLine); // Skip the first line (number of instances)
        int numJobs, numMachines, classNumber, instanceNumber;

        while (inputFile >> numJobs >> numMachines >> classNumber >> instanceNumber) {
            vector<int> tasks(numJobs);
            for (int i = 0; i < numJobs; ++i) {
                inputFile >> tasks[i];
            }

            outputFile << "Class " << classNumber << ", Instance " << instanceNumber << ":\n";
            mixedSPT_LPT(tasks, numMachines, outputFile);
        }

        inputFile.close();
        outputFile.close();
    }
