#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <iomanip>

#include <vector>
#include <map>

#include <signal.h>
#include <sys/wait.h>

#include <sys/resource.h>

#include "manipulatestring.h"

using namespace std;

#define LINE_LENGTH 100
#define MAX_BUF 128

void handleChild(int sig) {
    while (waitpid(-1, nullptr, WNOHANG) > 0);
}

void execFromChild(char * program, char ** cArgs, map<string, string> specialArgs) {
    int fdIn = STDIN_FILENO;
    int fdOut = STDOUT_FILENO;

    if (specialArgs["input_file"] != "") {
        if ((fdIn = open(strToChar(specialArgs["input_file"]), O_RDONLY)) < 0) {
            perror("Failed to 'open' input_file");
            _exit(-1);
        }
    }

    if (specialArgs["output_file"] != "") {
        if ((fdOut = open(strToChar(specialArgs["output_file"]), O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
            perror("Failed to 'open' output_file");
            _exit(-1);
        }
    }

    dup2(fdIn, STDIN_FILENO);
    dup2(fdOut, STDOUT_FILENO);

    execvp(program, cArgs);
    perror("Failed to execv");
    _exit(-1);
}

int runCommand(string input, map<string, vector<string>> &runningProcesses) {
    vector<string> args = splitString(input, ' ');
    map<string, string> specialArgs = getSpecialArgs(args);

    char ** cArgs = strVectorTocStringArray(args);
    char * program = strToChar(args[0]);

    signal(SIGCHLD, handleChild);

    int pid = fork();
    runningProcesses.insert({to_string(pid), {input, "R", "0"}});

    if (pid == -1) {
        perror("error: fork failed while running command");
        _exit(-1);
        
    } else if (pid == 0) {
        execFromChild(program, cArgs, specialArgs);
        _exit(-1);

    } else {
        if (specialArgs["background"] == "no") {
            wait(NULL);
        }
        return 0;
    }
}

string getPSOutput() {
    int fd[2], n;
    pid_t pid;
    char buffer[MAX_BUF];
    char command[8] = "/bin/ps";
    char * args[] = {command, NULL};

    string output = "";

    if (pipe(fd) < 0) {
        perror("'jobs' error: could not open pipe");
        _exit(-1);
    }

    if ((pid = fork()) < 0) {
        perror("'jobs' error: could not fork");
        _exit(-1);
    }
    
    if (pid ==0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);
        if (execvp(args[0], args) < 0) {
            perror("'jobs' error: could not execute");
            _exit(-1);
        }
    } else {
        close(fd[1]);
        while (((n = read(fd[0], buffer, MAX_BUF))) > 0) {
            buffer[n] = '\0';
            output += buffer;
        }  
        close(fd[0]);
    }
    return output;
}

map<string, vector<string>> parsePSOutput(string output) {
    map<string, vector<string>> jobInfos;
    
    vector<string> splitOutput = splitString(output, '\n');
    vector<string> lineTokens;

    splitOutput.erase(splitOutput.begin());

    for (string line: splitOutput) {
        lineTokens = splitString(line, ' ');
        jobInfos.insert({lineTokens[0], {lineTokens[2], lineTokens[3]}});  
    }

    return jobInfos;
}

void updateRunningProcesses(map<string, vector<string>> & runningProcesses) {
    string output = getPSOutput();
    map<string, vector<string>> jobInfos = parsePSOutput(output);
    
    struct tm tm;
    
    int time;
    

    for (map<string, vector<string>>::iterator it=runningProcesses.begin(); it != runningProcesses.end(); ) {

        if (jobInfos.find(it->first) == jobInfos.end()) {
            it =runningProcesses.erase(it);
        } else {
            istringstream ss(jobInfos[it->first][0]);
            ss >> get_time(&tm, "%H:%M:%S");
            time = tm.tm_hour * 3600 + tm.tm_min * 60 + tm.tm_sec;

            // Update process block with the program's running time from 'ps'
            (it->second)[2] = to_string(time);
            ++it;
        }
        
    }
}

int dispTime() {
    struct rusage usage;
    string userTime, systemTime;

    if (getrusage(RUSAGE_CHILDREN, &usage) < 0) {
        perror("getrusage");
        return -1;
    }
    userTime = to_string(usage.ru_utime.tv_sec);
    systemTime = to_string(usage.ru_stime.tv_sec);

    cout << "User time = " << userTime << " seconds" << endl;
    cout << "System time = " << systemTime << " seconds" << endl;

    return 0;
}

int displayProcessInfo(map<string, vector<string>> runningProcesses) {
    string runSleep, command, psTime;
    int i = 0;

    // display running process info
    cout << "Running Processes: " << endl;
    cout << "#" << setfill(' ') << setw(12) << "PID " << "S " << "SEC " << "COMMAND" << endl;
    for (auto kv: runningProcesses) {
        command = kv.second[0];
        runSleep = kv.second[1];
        psTime = kv.second[2];
        cout << i++ << setfill(' ') << setw(4) << " " << kv.first << " " << runSleep << " " << psTime << " " << command << endl;
    }
    cout << "Processes = " << runningProcesses.size() << " active" << endl;

    // display completed process info
    cout << "Completed processes:" << endl;
    dispTime();

    return 0;
}

void waitForChildren() {
    while (wait(NULL) > 0);
}

bool pidExists(string pid, map<string, vector<string>> runningProcesses) {
    if (runningProcesses.find(pid) == runningProcesses.end()) {
        cerr << "PID does not exist" << endl;
        return false;
    };

    return true;
}

int killProcess(vector<string> inputSplit, map<string, vector<string>> runningProcesses) {
    if (inputSplit.size() != 2) {
        cerr << "PID required" << endl;
        return -1;
    } 

    if (!pidExists(inputSplit[1], runningProcesses)) return -1;

    kill(stoi(inputSplit[1]), SIGKILL);

    return 0;
}

int waitForProcess(vector<string> inputSplit, map<string, vector<string>> runningProcesses) {
    if (inputSplit.size() != 2) {
        cerr << "PID required" << endl;
        return -1;
    } 

    if (!pidExists(inputSplit[1], runningProcesses)) return -1;

    waitpid(stoi(inputSplit[1]), nullptr, WUNTRACED);

    return 0;
}

int suspendProcess(vector<string> inputSplit, map<string, vector<string>> & runningProcesses) {
    if (inputSplit.size() != 2) {
        cerr << "PID required" << endl;
        return -1;
    } 

    if (!pidExists(inputSplit[1], runningProcesses)) return -1;

    kill(stoi(inputSplit[1]), SIGSTOP);
    runningProcesses[inputSplit[1]][1] = "S";

    return 0;
}

int resumeProcess(vector<string> inputSplit, map<string, vector<string>> & runningProcesses) {
    if (inputSplit.size() != 2) {
        cerr << "PID required" << endl;
        return -1;
    } 

    if (!pidExists(inputSplit[1], runningProcesses)) return -1;

    kill(stoi(inputSplit[1]), SIGCONT);
    runningProcesses[inputSplit[1]][1] = "R";

    return 0;
}

int sleepProcesses(vector<string> inputSplit) {
    if (inputSplit.size() != 2) {
        cerr << "Sleep time required" << endl;
        return -1;
    }

    sleep(stoi(inputSplit[1])); 

    return 0;
}

