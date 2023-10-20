#include <vector>
#include <string>
#include <map>

using namespace std;

char * strToChar(string s);

char ** strVectorTocStringArray(vector<string> strVector);

vector<string> splitString(string str, char delimiter);

map<string, string> getSpecialArgs(vector<string> & args);

