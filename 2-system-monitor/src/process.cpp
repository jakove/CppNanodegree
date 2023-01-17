#include "process.h"

#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_{pid}
{
  std::string uid = LinuxParser::Uid(pid);
  user_ = LinuxParser::User(pid);
  command_ = LinuxParser::Command(pid);
  uptime_ = LinuxParser::UpTime(pid);
  cpu_utilization_ = CalculateCpuUtilization();
  ram_ = LinuxParser::Ram(pid);
};

int Process::Pid() { return pid_; }

float Process::CpuUtilization() const { return cpu_utilization_; }

string Process::Command() const { return command_; }

string Process::Ram() const { return ram_; }

string Process::User() const { return user_; }

long int Process::UpTime() const { return uptime_; }

bool Process::operator<(Process const &a) const
{
  return this->cpu_utilization_ < a.cpu_utilization_;
}

bool Process::operator>(Process const &a) const
{
  return this->cpu_utilization_ > a.cpu_utilization_;
}

double Process::CalculateCpuUtilization()
{
  auto current_active_jiffies = LinuxParser::ActiveJiffies(pid_);
  auto delta_jiffies = current_active_jiffies - active_jiffies_;
  auto current_total_jiffies =
      LinuxParser::ActiveJiffies("cpu") + LinuxParser::IdleJiffies("cpu");
  auto delta_total_jiffies = current_total_jiffies - total_jiffies_;

  total_jiffies_ = current_total_jiffies;
  active_jiffies_ = current_active_jiffies;
  if (delta_total_jiffies > 0)
  {
    return static_cast<double>(delta_jiffies) /
           static_cast<double>(delta_total_jiffies);
  }
  return 0.0;
}

void Process::Update()
{
  cpu_utilization_ = CalculateCpuUtilization();
  ram_ = LinuxParser::Ram(pid_);
  updated_ = true;
}

bool Process::HasUpdated()
{
  bool status = updated_;
  updated_ = false;
  return status;
}