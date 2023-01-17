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

Processor &System::Cpu()
{
    return cpu_;
}

vector<Process> &System::Processes()
{
    const auto &pids = LinuxParser::Pids();
    for (const auto &pid : pids)
    {
        // check if pid is an old process
        const auto it = std::find_if(processes_.begin(), processes_.end(),
                                     [pid](Process &this_process)
                                     { return this_process.Pid() == pid; });
        if (it != processes_.end())
        {
            it->Update();
        }
        else
        {
            Process new_process(pid);
            if (new_process.Command() != "")
            {
                processes_.push_back(new_process);
            }
        }
    }
    // after round about 60 seconds loop over all processes and check if it was updated since last call.
    if (rounds_ >= 60)
    {
        // if not the process isn't active anymore so it has to be erased from the list
        std::vector<std::vector<Process>::iterator> to_erase;
        for (auto it = processes_.begin(); it != processes_.end(); it++)
        {
            if (!it->HasUpdated())
            {
                to_erase.push_back(it);
            }
        }

        for (const auto &erase_it : to_erase)
        {
            processes_.erase(erase_it);
        }
        rounds_ = 0;
    }
    // sort cpuutilization descending
    std::sort(processes_.begin(), processes_.end(), [](const Process &a, const Process &b)
              { return a > b; });

    rounds_++;
    return processes_;
}

std::string System::Kernel()
{
    return LinuxParser::Kernel();
}

float System::MemoryUtilization()
{
    return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem()
{
    return LinuxParser::OperatingSystem();
}

int System::RunningProcesses()
{
    return LinuxParser::RunningProcesses();
}

int System::TotalProcesses()
{
    return LinuxParser::TotalProcesses();
}

long int System::UpTime()
{
    return LinuxParser::UpTime();
}
