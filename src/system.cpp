#include <pstl/glue_algorithm_defs.h>
#include <unistd.h>
#include <algorithm>
#include <iterator>
#include <set>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() : kernel_(LinuxParser::Kernel()), OS_(LinuxParser::OperatingSystem()), pids_(LinuxParser::Pids()) {
    for (const auto& pid : pids_)
        processes_.emplace_back(Process(pid));
}

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
    /* std::sort(pids_.begin(), pids_.end()); */

    // LinuxParser::Pids() returns always sorted vector
    std::vector<int> currentPids = LinuxParser::Pids();

    if (pids_.size() != currentPids.size() && !std::equal(pids_.begin(), pids_.end(), currentPids.begin())) {
        std::vector<int> symDiff;
        /* Find different processes in each iteration (returns also a sorted vector)
           which are either the new ones or the disappeared. Keep the running ones */
        std::set_symmetric_difference(
                pids_.begin(), pids_.end(),
                currentPids.begin(), currentPids.end(),
                std::back_inserter(symDiff));
        if (pids_.size() < currentPids.size()) {
            for (const auto& sd : symDiff)
                processes_.emplace_back(Process(sd));
        }
        else {
            // Erase-remove idiom to get rid of disappeared processes
            processes_.erase(std::remove_if(processes_.begin(), processes_.end(),
                        [&] (Process& p)
                        { return std::binary_search(symDiff.begin(), symDiff.end(), p.Pid()); }),
                    processes_.end());
        }
    }

    pids_ = std::move(currentPids);
    std::sort(processes_.begin(), processes_.end(), [&] (Process const& a, Process const& b) { return a < b; });
    return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return kernel_; }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() { return OS_; }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
