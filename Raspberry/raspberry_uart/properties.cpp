#include "properties.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

Properties::Properties(){}

Properties::Properties(std::string filename){
	load(filename);
}

void Properties::load(const std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        // File does not exist, try to create it
        std::ofstream newFile(filename);
        if (!newFile.is_open()) {
            throw std::runtime_error("Unable to open or create file: " + filename);
        }
        // File created successfully, close it
        newFile.close();
        return; // Exit after creating the file
    }
	path = filename;

    // Load properties from the existing file
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream lineStream(line);
        std::string key, value;
        if (std::getline(lineStream, key, '=') && std::getline(lineStream, value)) {
            properties[key] = value;
        }
    }
    file.close();
}

void Properties::save(const std::string filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }
    
    for (const auto& pair : properties) {
        file << pair.first << "=" << pair.second << "\n";
    }
    file.close();
}

void Properties::flush() const {
	save(path);
}


void Properties::addWhenMissing(bool s){
	awm = s;
}

std::string Properties::get(const std::string key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return it->second;
    }
    return "";
}

std::string Properties::get(const std::string key, const std::string defaultValue) {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return it->second;
    }
	if(awm) set(key, defaultValue);
    return defaultValue;
}

void Properties::set(const std::string key, const std::string value) {
    properties[key] = value;
}

void Properties::remove(const std::string key) {
    properties.erase(key);
}

int Properties::getInt(const std::string key, int defaultValue) {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return std::stoi(it->second);
    }
	if(awm) setInt(key, defaultValue);
    return defaultValue;
}

void Properties::setInt(const std::string key, int value) {
    properties[key] = std::to_string(value);
}

long Properties::getLong(const std::string key, long defaultValue) {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return std::stol(it->second);
    }
	if(awm) setLong(key, defaultValue);
    return defaultValue;
}

void Properties::setLong(const std::string key, long value) {
    properties[key] = std::to_string(value);
}

float Properties::getFloat(const std::string key, float defaultValue) {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return std::stof(it->second);
    }
	if(awm) setFloat(key, defaultValue);
    return defaultValue;
}

void Properties::setFloat(const std::string key, float value) {
    properties[key] = std::to_string(value);
}

bool Properties::getBool(const std::string key, bool defaultValue) {
    auto it = properties.find(key);
    if (it != properties.end()) {
        std::string value = it->second;
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        if (value == "true" || value == "1") {
            return true;
        } else if (value == "false" || value == "0") {
            return false;
        }
    }
	if(awm) setBool(key, defaultValue);
    return defaultValue;
}

void Properties::setBool(const std::string key, bool value) {
    properties[key] = value ? "true" : "false";
}
