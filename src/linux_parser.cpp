#include <unistd.h>
#include <algorithm>
#include <sstream>
#include <vector>
#include <filesystem>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
    string line;
    string key;
    string value;
    std::ifstream filestream(kOSPath);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ' ', '_');
            std::replace(line.begin(), line.end(), '=', ' ');
            std::replace(line.begin(), line.end(), '"', ' ');
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "PRETTY_NAME") {
                    std::replace(value.begin(), value.end(), '_', ' ');
                    return value;
                }
            }
        }
    }
    return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
    string os, version, kernel;
    string line;
    std::ifstream stream(kProcDirectory + kVersionFilename);
    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> os >> version >> kernel;
    }
    return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
    std::vector<int> pids;
    for (const auto& d : std::filesystem::directory_iterator(kProcDirectory)) {
        const auto filename = d.path().filename().string();
        if (std::all_of(filename.begin(), filename.end(), ::isdigit))
            pids.push_back(std::stoi(filename));
    }
    return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
    std::string line, key, value;
    float MemAvailable = 0.0, MemTotal = 1.0;

    std::ifstream filestream(kProcDirectory + kMeminfoFilename);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "MemTotal:") {
                    try { MemTotal = stof(value); }
                    catch (...) { MemTotal = 0; }
                }
                else if (key == "MemAvailable:") {
                    try { MemAvailable = stof(value); }
                    catch (...) { MemAvailable = 0; }
                    break;
                }
            }
        }
    }
    return 1.0 - MemAvailable / MemTotal;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
    std::string ret;
    if (std::ifstream(kProcDirectory + kUptimeFilename) >> ret)
        return std::stol(ret, nullptr, 10);
    else
        return 0;
}
//------------------------------------------------------------------------------
// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }
//------------------------------------------------------------------------------

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
    std::vector<std::string> ret;
    std::string line, token;
    std::ifstream filestream(kProcDirectory + kStatFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        linestream >> token;
        for (int i = 0; i < 10; ++i) {
            linestream >> token;
            ret.emplace_back(token);
        }
    }
    return ret;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
    std::string ret = GetValue("processes", kProcDirectory + kStatFilename);
    try { return std::stoi(ret); }
    catch (...) { return 0; }
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
    std::string ret = GetValue("procs_running", kProcDirectory + kStatFilename);
    try { return std::stoi(ret, nullptr, 10); }
    catch (...) { return 0; }
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
    std::string line;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
    }
    return line;
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
    std::string ret = GetValue("VmSize:", kProcDirectory + to_string(pid) + kStatusFilename);
    int tmp;
    try { tmp = std::stoi(ret, nullptr, 10); }
    catch (...) { tmp = 0; }
    return to_string(tmp / 1024);
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
    return GetValue("Uid:", kProcDirectory + to_string(pid) + kStatusFilename);
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
    std::string line, uid = ":" + Uid(pid) + ":";
    std::ifstream filestream(kPasswordPath);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            if (line.find(uid) != std::string::npos)
                return line.substr(0, line.find(":"));
        }
    }
    return string();
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
    std::string line, token;
    std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        for (int i = 0; i < 22; ++i)
            linestream >> token;
    }
    try { return std::stol(token, nullptr, 10) / sysconf(_SC_CLK_TCK); }
    catch (...) { return 0; }
}


// Own helper function
std::string LinuxParser::GetValue(std::string key, std::string path) {
    std::string line, k, value;
    std::ifstream filestream(path);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::istringstream linestream(line);
            while (linestream >> k >> value) {
                if (key == k)
                    return value;
            }
        }
    }
    return std::string();
}
