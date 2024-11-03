#include <iostream>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <random>
using namespace std;

// Include each script file
#include "folder1/lpt_script.cpp"
#include "folder2/spt_script.cpp"
#include "folder3/mixed_lpt_spt_script.cpp"
#include "folder4/mixed_spt_lpt_script.cpp"

void generateInputFile(int numJobs, int numMachines, int numClasses, int instancesPerClass, int maxLoad) {
    ofstream inputFile("input.txt");
    if (!inputFile) {
        cerr << "Failed to create input.txt" << endl;
        exit(1);
    }

    inputFile << maxLoad << endl;

    mt19937 gen(42); // Use fixed seed for reproducibility if random_device fails
    uniform_int_distribution<> dis(0, maxLoad);

    for (int classNum = 1; classNum <= numClasses; ++classNum) {
        for (int instance = 1; instance <= instancesPerClass; ++instance) {
            inputFile << numJobs << " " << numMachines << " " << classNum << " " << instance << endl;
            for (int job = 0; job < numJobs; ++job) {
                inputFile << dis(gen) << (job != numJobs - 1 ? " " : "\n");
            }
        }
    }
    inputFile.close();
    cout << "Input file generated: input.txt" << endl;
}

int main() {
    // Step 1: Generate input file
    int numJobs = 10, numMachines = 2, numClasses = 2, instancesPerClass = 5, maxLoad = 650;
    generateInputFile(numJobs, numMachines, numClasses, instancesPerClass, maxLoad);

    // Step 2: Create output directory if it doesn't exist
    struct stat info;
    if (stat("output", &info) != 0) {
        system("mkdir output");
    }

    // Step 3: Call each function directly
    runLPT();
    runSPT();
    runMixedLPTSPT();
    runMixedSPTLPT();

    cout << "All scripts executed. Check the output folder for results." << endl;
    return 0;
}
