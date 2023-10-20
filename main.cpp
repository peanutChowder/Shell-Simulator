#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <unistd.h>

#include "manipulatestring.h"
#include "jobs.h"

#define LINE_LENGTH 100
#define MAX_ARGS 7
#define MAX_LENGTH 20
#define MAX_PT_ENTRIES 32

using namespace std;

int main(void) {
    string input;
    vector<string> inputTokens;
    vector<string> args;
    int argc;

    map<string, vector<string>> runningProcesses;


    while (1) {
        cout << "SHELL379: ";
        
        // Get input
        getline(cin, input);

        // Skip loop if user entered nothing
        if (input.length() == 0) {
            continue;
        }

        inputTokens = splitString(input, ' ');

        if (input == "exit") {
            // Resume all children to avoid getting stuck
            for (auto kv: runningProcesses) {
                resumeProcess({"", kv.first}, runningProcesses);
            }

            waitForChildren();
            cout << "Resources used" << endl;
            dispTime();
            return 0;

        } else if (input == "jobs") {
            updateRunningProcesses(runningProcesses);
            displayProcessInfo(runningProcesses);
        } else if (inputTokens[0] == "kill") {
            killProcess(inputTokens, runningProcesses);
        } else if (inputTokens[0] == "wait") {
            waitForProcess(inputTokens, runningProcesses);
        } else if (inputTokens[0] == "suspend") {
            suspendProcess(inputTokens, runningProcesses);
        } else if (inputTokens[0] == "resume") {
            resumeProcess(inputTokens, runningProcesses);
        } else if (inputTokens[0] == "sleep") {
            sleepProcesses(inputTokens);
        } else {
            runCommand(input, runningProcesses);         
        }
    }
}