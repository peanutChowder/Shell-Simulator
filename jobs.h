#include <vector>
#include <string>
#include <map>

using namespace std;

int runCommand(string input, map<string, vector<string>> &runningProcesses) ;

string getPSOutput();

map<string, vector<string>> parsePSOutput(string output);

void updateRunningProcesses(map<string, vector<string>> & runningProcesses);

int dispTime();

int displayProcessInfo(map<string, vector<string>> runningProcesses);

void waitForChildren();

int killProcess(vector<string> inputSplit, map<string, vector<string>> runningProcesses);

int waitForProcess(vector<string> inputSplit, map<string, vector<string>> runningProcesses);

int suspendProcess(vector<string> inputSplit, map<string, vector<string>> & runningProcesses);

int resumeProcess(vector<string> inputSplit, map<string, vector<string>> & runningProcesses);

int sleepProcesses(vector<string> inputSplit);