#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <future>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

/**
 * Reads JSON source file into a `json` object in memory.
 */
json loadJsonFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return json();
    }

    json jsonData;
    try {
        file >> jsonData;  
    } catch (const std::exception& e) {
        std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
    }

    return jsonData;
}

/**
 * Searches JSON object for elements matching key-value pair and writes results to a file.
 */
void processKeyValueSearch(const json& root, const std::string& searchKey, const std::string& searchValue, const std::string& outFilename, std::vector<std::string>& searchResultStrings) {
    std::string outString;
    json results = json::array();  
    bool found = false;

    for (const auto& item : root) {
        if (item[searchKey] == searchValue) {
            results.push_back(item);
            found = true;
        }
    }

    if (!found) {
        searchResultStrings.push_back("Results not found for '" + searchKey + "=" + searchValue + "'");
    }else{
        searchResultStrings.push_back("Results found for '" + searchKey + "=" + searchValue + "'");
        std::ofstream outFile(outFilename);
        if (!outFile) {
            searchResultStrings.push_back("Error: Unable to open output file '" + outFilename + "'");
            return;
        }

        outString = results.dump(4);
        outFile << outString << std::endl;  
        outFile.flush();
        outFile.close();
    }
}

/**
 * Searches JSON object for elements matching search term string and writes results to a file.
 */
void processSearchString(const json& root, const std::string& searchTerm, const std::string& outFilename, std::vector<std::string>& searchResultStrings) {
    std::string outString;
    json results = json::array();  
    bool found = false;

    for (const auto& item : root) {
        std::string jsonStr = item.dump();  

        if (jsonStr.find(searchTerm) != std::string::npos) {
            results.push_back(item);
            found = true;
        }
    }

    if (!found) {
        searchResultStrings.push_back("Results not found for '" + searchTerm + "'");
    }else{
        searchResultStrings.push_back("Results found for '" + searchTerm + "'");
        std::ofstream outFile(outFilename);
        if (!outFile) {
            searchResultStrings.push_back("Error: Unable to open output file '" + outFilename + "'");
            return;
        }

        outString = results.dump(4);
        outFile << outString << std::endl;  
        outFile.flush();
        outFile.close();
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <json_file> <search_string1> [search_string2] ...\n";
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];
    
    auto loadStart = std::chrono::high_resolution_clock::now();

    json root = loadJsonFile(filename);
    if (root.is_null()) {
        return EXIT_FAILURE;
    }

    auto loadDuration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - loadStart);
    std::cout << std::endl;
    std::cout << "JSON load duration: " << loadDuration.count() << " seconds" << std::endl;

    std::vector<std::future<void>> futures;
    std::vector<std::string> searchResultStrings;

    auto searchStart = std::chrono::high_resolution_clock::now();

    for (int i = 2; i < argc; i++) {
        std::string searchTerm = argv[i];
        std::string outFilename = "search_result_" + searchTerm + ".json";
        if(searchTerm.find("=") != std::string::npos){
            futures.push_back(std::async(std::launch::async, processKeyValueSearch, std::ref(root), searchTerm.substr(0, searchTerm.find("=")), searchTerm.substr(searchTerm.find("=")+1, searchTerm.length()), outFilename, std::ref(searchResultStrings))); 
        }else{
            futures.push_back(std::async(std::launch::async, processSearchString, std::ref(root), searchTerm, outFilename, std::ref(searchResultStrings)));
        }
    }

    for (auto& f : futures) {
        f.get();
    }

    for (auto i : searchResultStrings){
        std::cout << i <<std::endl;
    }

    auto searchDuration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - searchStart);
    std::cout << "Search duration: " << searchDuration.count() << " seconds" << std::endl;
    std::cout << std::endl;

    return EXIT_SUCCESS;
}
