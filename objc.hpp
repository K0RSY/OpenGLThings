#ifndef OBJC
#define OBJC 1

#include <vector>
#include <algorithm>
#include <string>
#include <fstream>

#define OBJC_STRIDE 8
#define objc_pointer(value) &(value[0])

using namespace std;

typedef struct {
    vector<float> vertecies;
    vector<uint> indices;
} objc;

// Define
vector<string> objc_split(string str, const char* delimiter, uint delimiter_size = 1);
vector<string> objc_sublist_to_end(vector<string> list, uint start = 1);
vector<float> objc_strl_to_floatl(vector<string> list);
void objc_append_list(vector<float>* list, vector<float> list_to_append); // TODO: Fix this shit with templates

objc objc_init(const char* path);

// Implement
vector<string> objc_split(string str, const char* delimiter, uint delimiter_size) {
    vector<string> result = {};

    int index;
    while ((index = str.find(delimiter)) != str.npos) {
        result.push_back(str.substr(0, index));
        str = str.substr(index + delimiter_size, str.npos);
    };
    result.push_back(str);

    return result;
};

vector<string> objc_sublist_to_end(vector<string> list, uint start) {
    return vector<string>(list.begin() + start, list.end());
};

vector<float> objc_strl_to_floatl(vector<string> list) {
    vector<float> result = {};

    for (string str : list) {
        result.push_back(stof(str));
    }

    return result;
};

void objc_append_list(vector<float>* list, vector<float> list_to_append) {
    for (uint i = 0; i < list_to_append.size(); i++) {
        (*list).push_back(list_to_append[i]);
    }
};

objc objc_init(const char* path) {
    objc object;

    ifstream file_stream = ifstream(path);
    string line;
    vector<string> line_substr = {};

    vector<vector<float>> v = {};
    vector<vector<float>> vt = {};
    vector<vector<float>> vn = {};

	if (file_stream.is_open()) {
        while (getline(file_stream, line)) {
            line_substr = objc_split(line, " ");

            if (line_substr.size() <= 1) {
                continue;
            } else if (line_substr[0] == "v") {
                v.push_back(objc_strl_to_floatl(objc_sublist_to_end(line_substr)));
            } else if (line_substr[0] == "vt") {
                vt.push_back(objc_strl_to_floatl(objc_sublist_to_end(line_substr)));
            } else if (line_substr[0] == "vn") {
                vn.push_back(objc_strl_to_floatl(objc_sublist_to_end(line_substr)));
            } else if (line_substr[0] == "f") {
                auto vertex_part = objc_sublist_to_end(line_substr, 1);

                for (auto raw_vertex : vertex_part) {
                    auto vertex = objc_strl_to_floatl(objc_split(raw_vertex, "/"));

                    objc_append_list(&(object.vertecies), v[vertex[0] - 1]);
                    objc_append_list(&(object.vertecies), vn[vertex[2] - 1]);
                    objc_append_list(&(object.vertecies), vt[vertex[1] - 1]);
                }
                uint last_vertex_index = object.vertecies.size() / OBJC_STRIDE - 1;

                switch (vertex_part.size()) {
                    case 3:
                        object.indices.push_back(last_vertex_index - 2);
                        object.indices.push_back(last_vertex_index - 1);
                        object.indices.push_back(last_vertex_index);
                        break;

                    case 4:
                        object.indices.push_back(last_vertex_index - 2);
                        object.indices.push_back(last_vertex_index - 1);
                        object.indices.push_back(last_vertex_index);

                        object.indices.push_back(last_vertex_index - 2);
                        object.indices.push_back(last_vertex_index);
                        object.indices.push_back(last_vertex_index - 3);
                        break;
                    
                    default:
                        break;
                };
            };
        }

		file_stream.close();
    }

    return object;
};

#endif