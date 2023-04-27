#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

// Sort vector
#include <algorithm>

#include <filesystem> // C++17
namespace fs = std::filesystem;

using namespace std;

ostream& operator<<(ostream& os, const vector<string>& vec) {
    for (const auto& str: vec) {
        os << str << endl;
    }
    return os;
}

int main(int argc, char* argv[]) {
    vector<string> arguments;

    // Iterate arguments (argument 0 is the executable path)
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        arguments.push_back(arg);
    }

    vector<string> invalidDirs;
    vector<string> validDirs;

    // Process arguments
    for (size_t i = 0; i < arguments.size(); i++) {
        if (arguments[i] == "-h" || arguments[i] == "--help") {
            cout << "Usage: Gamelist.exe [OPTIONS] [Game library paths]" << endl;
            cout << "Options:" << endl;
            cout << "-h, --help     Display this help message" << endl;
            cout << "-v, --version  Display version information" << endl;
            return 0;
        } else if (arguments[i] == "-v" || arguments[i] == "--version") {
            cout << "Gamelist v1.5" << endl << endl;
            cout << "---------------------------------------------------------------------------------" << endl << endl;
            cout << "MIT License" << endl << endl;
            cout << "Copyright (c) 2023 Mathias K. E. Sørensen" << endl << endl;
            cout << "Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:" << endl << endl;
            cout << "The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software." << endl << endl;
            cout << "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE." << endl;
            return 0;
        } else {
            if (!fs::is_directory(arguments[i])) {
                // Push back invalid directories for later processing
                invalidDirs.push_back(arguments[i]);
                continue;
            }
            
            // If valid directory
            validDirs.push_back(arguments[i]);
        }
    }

    if (validDirs.size() < 1 && arguments.size() < 1) {
        ifstream file("defaultpaths.txt");
        
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (fs::is_directory(line)) {
                    validDirs.push_back(line);
                } else {
                    invalidDirs.push_back(line);
                }
            }
            file.close();
        }
        // If file couldnt be opened
        else {
            cout << "Could not find defaultpaths.txt" << endl << "Creating one, simply insert library paths separated by linebreaks" << endl;
            ofstream outfile("defaultpaths.txt");
            outfile.close();
            return 0;
        }
    }

    if (validDirs.size() > 0) {
        cout << "Folder sizes:" << endl;
        for (size_t i = 0; i < validDirs.size(); i++) {
            // Iterate over all the files and subdirectories in the folder
            uintmax_t total_size = 0;
            for (const auto& entry : fs::recursive_directory_iterator(validDirs[i])) {
                // Add the size of each file to the total size
                if (fs::is_regular_file(entry)) {
                    total_size += fs::file_size(entry);
                }
            }

            // Convert byte to megabyte
            cout << "\033[44m" << validDirs[i] << " | " << (total_size / 1000000) << " MB (" << total_size << " bytes)" << "\033[0m" << endl;

            // Create vector of pairs to later sort by size
            vector<pair<string, uintmax_t>> games;
            
            // Get game sizes
            for (const auto& folder: fs::directory_iterator(validDirs[i])) {
                if (folder.is_directory()) {
                    string filename = folder.path().filename().string();
                    uintmax_t game_size = 0;
                    for (const auto& game : fs::recursive_directory_iterator(folder)) {
                        if (fs::is_regular_file(game)) {
                            game_size += fs::file_size(game);
                        }
                    }
                    games.push_back(make_pair(filename.substr(filename.find_last_of("\\") + 1), game_size));
                }
            }

            // Sort by size
            sort(games.begin(), games.end(), [](const pair<string, uintmax_t>& a, const pair<string, uintmax_t>& b){
                return a.second > b.second;
            });

            int longestLength = 0;
            for (auto& p : games) {
                int len = p.first.length();
                if (len > longestLength) {
                    longestLength = len;
                }
            }

            // Output games
            for (auto game : games) {
                cout << "  \033[32m" << left << setw(longestLength+5) << game.first << "\033[0m " << setw(0) << to_string(game.second / 1000000) + " MB" << " (" + to_string(game.second) + " bytes)" << endl;
            }

            if (i != validDirs.size()-1)
                cout << endl;
        }
    } else {
        cout << "\033[31m" << "No folders given" << "\033[0m" << endl;
    }

    // If there is any invalid directories
    if (!invalidDirs.empty()) {
        cout << endl << "\033[31m" << "The following director" << (invalidDirs.size() > 1 ? "ies are" : "y is") << " invalid:" << "\033[0m" << endl;
        for (size_t i = 0; i < invalidDirs.size(); i++) {
            cout << "  \033[44m" << invalidDirs[i] << "\033[0m" << endl;
        }
        cout << endl << "\033[31m" << "Try encapsulating the path in quotation marks, example: \"C:\\Program Files (x86)\\Steam\\steamapps\\common\"" << "\033[0m" << endl;
    }

    return 0;
}