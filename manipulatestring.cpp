#include <vector>
#include <string>
#include <cstring>
#include <sstream>

#include <map>

using namespace std;

char * strToChar(string s) {
    char * cString = new char[s.length() + 1];

    strcpy(cString, s.c_str());

    return cString;
}

char ** strVectorTocStringArray(vector<string> strVector) {
    char ** cStringArray = (char **) malloc(sizeof(char *) * (strVector.size() + 1));
    char * cString;
    int i = 0;

    for (string s: strVector) {
        cString = strToChar(s);
        cStringArray[i++] = cString;
    }

    cStringArray[i] = NULL;

    return cStringArray;
}

vector<string> splitString(string str, char delimiter) {
    vector<string> strVector;
    string token;
    stringstream ss(str);
    
    while (getline(ss, token, delimiter)) {
        // Don't add empty strings
        if (token != "") {
            strVector.push_back(token);
        }  
    }

    return strVector;
}

map<string, string> getSpecialArgs(vector<string> & args) {
    map<string, string> specialArgs = {
        {"input_file", ""},
        {"output_file", ""},
        {"background", "no"}
    };

    for (vector<string>::iterator it = args.begin(); it != args.end(); ) {
        if ((*it)[0] == '<') {
            specialArgs["input_file"] = (*it).substr(1);
            args.erase(it);

        } else if ((*it)[0] == '>') {
            specialArgs["output_file"] = (*it).substr(1);
            args.erase(it);

        } else if ((*it) == "&") {
            specialArgs["background"] = "yes";
            args.erase(it);

        } else {
            ++it;
        }
    }

    return specialArgs;
}

