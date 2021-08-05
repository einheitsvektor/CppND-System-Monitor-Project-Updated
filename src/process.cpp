#include <unistd.h>
#include <sstream>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid), user_(LinuxParser::User(pid)), command_(LinuxParser::Command(pid)) {
    cpu_ = CpuUtilization();
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    std::string line,  token;
    float utime = 0, stime = 0, cutime = 0, cstime = 0, starttime = 0;

    std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid_) + LinuxParser::kStatFilename);
    if (filestream.is_open()) {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        for (int i = 1; i <= 22; ++i) {
            linestream >> token;
            switch (i) {
                case 14:
                    utime = std::stof(token); break;
                case 15:
                    stime = std::stof(token); break;
                case 16:
                    cutime = std::stof(token); break;
                case 17:
                    cstime = std::stof(token); break;
                case 22:
                    starttime = std::stof(token); break;
                default: break;
            }
        }
    }
    float total_time = utime + stime + cutime + cstime;
    float Hertz = static_cast<float>(sysconf(_SC_CLK_TCK));
    float seconds = static_cast<float>(LinuxParser::UpTime()) - (starttime / Hertz);
    cpu_ = (total_time / Hertz) / seconds;
    return cpu_;
}

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return a.cpu_ < cpu_; }
