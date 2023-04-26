#include <iostream>
#include <fstream>
#include <string>
#include <vector>

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
            cout << "Gamelist v1.0" << endl << endl;
            cout << "---------------------------------------------------------------------------------" << endl << endl;
            ifstream file("LICENSE");
        
            if (file.is_open()) {
                string line;
                while (getline(file, line)) {
                    cout << line << endl;
                }
                file.close();
            }
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
        cout << "No arguments given, trying default paths" << endl;
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
            cout << "Could not find defaultpaths.txt" << endl;
            return 0;
        }
    }

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
        cout << validDirs[i] << " | " << (total_size / 1000000) << " MB (" << total_size << " bytes)" << endl;
        
        for (const auto& folder: fs::directory_iterator(validDirs[i])) {
            if (folder.is_directory()) {
                string filename = folder.path().filename().string();
                uintmax_t game_size = 0;
                for (const auto& game : fs::recursive_directory_iterator(folder)) {
                    if (fs::is_regular_file(game)) {
                        game_size += fs::file_size(game);
                    }
                }
                cout << "  " << filename.substr(filename.find_last_of("\\") + 1) << " | " << (game_size / 1000000) << " MB (" << game_size << " bytes)" << endl;
            }
        }
        if (i != validDirs.size()-1)
            cout << endl;
    }

    // If there is any invalid directories
    if (!invalidDirs.empty()) {
        cout << endl << "The following director" << (invalidDirs.size() > 1 ? "ies are" : "y is") << " invalid:"<< endl;
        for (size_t i = 0; i < invalidDirs.size(); i++) {
            cout << "  " << invalidDirs[i] << endl;
        }
        cout << endl << "Try encapsulating the path in quotation marks, example: \"C:\\Program Files (x86)\\Steam\\steamapps\\common\"";
    }

    return 0;
}