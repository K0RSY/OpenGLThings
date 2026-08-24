#ifndef RDFL
#define RDFL 1

#include <string>
#include <fstream>

using namespace std;

string rdfl(const char* path) {
    ifstream file_stream = ifstream(path);
    string line;
    string result;

	if (file_stream.is_open()) {
        while (getline(file_stream, line)) {
            result.append(line + "\n");
        }

		file_stream.close();
    }

    return result;
}

#endif