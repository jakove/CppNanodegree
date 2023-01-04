#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

// TODO: Return the system's CPU
Processor& System::Cpu() { 
    return cpu_; 
}

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    const auto& pids = LinuxParser::Pids(); 
    for(const auto& pid : pids) {
        // check if pid is an old process
        const auto it = std::find_if(processes_.begin(), processes_.end(), 
            [pid](Process& this_process) {return this_process.Pid() == pid; });
        
        if(it != processes_.end()) {        
            it->Update();
        }
        else {
            Process new_process(pid);
            if(new_process.Command() != "") {
                processes_.push_back(new_process);
            }
        }
    }
    // loop over all processes and check if it was updated since last call.
    // if not the process isn't active anymore
    for(auto it = processes_.begin(); it != processes_.end(); it++) {
        if(!it->HasUpdated()) {
            processes_.erase(it);
        }
    }
    // sort cpuutilization descending 
    std::sort(processes_.begin(), processes_.end(), [](const Process& a, const Process& b) {return a > b; });
    return processes_; 
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() {  
    return LinuxParser::Kernel(); 
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { 
    return LinuxParser::MemoryUtilization(); 
}

// TODO: Return the operating system name
std::string System::OperatingSystem() { 
    return LinuxParser::OperatingSystem();     
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { 
    return LinuxParser::RunningProcesses(); 
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { 
    return LinuxParser::TotalProcesses(); 
}

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { 
    return LinuxParser::UpTime(); 
}
