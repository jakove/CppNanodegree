#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser
{
  // Paths
  const std::string kProcDirectory{"/proc/"};
  const std::string kCmdlineFilename{"/cmdline"};
  const std::string kCpuinfoFilename{"/cpuinfo"};
  const std::string kStatusFilename{"/status"};
  const std::string kStatFilename{"/stat"};
  const std::string kUptimeFilename{"/uptime"};
  const std::string kMeminfoFilename{"/meminfo"};
  const std::string kVersionFilename{"/version"};
  const std::string kOSPath{"/etc/os-release"};
  const std::string kPasswordPath{"/etc/passwd"};

  // System
  float MemoryUtilization();
  long UpTime();
  std::vector<int> Pids();
  int TotalProcesses();
  int RunningProcesses();
  std::string OperatingSystem();
  std::string Kernel();

  // CPU
  enum CPUStates
  {
    kUser_ = 0,
    kNice_,
    kSystem_,
    kIdle_,
    kIOwait_,
    kIRQ_,
    kSoftIRQ_,
    kSteal_,
    kGuest_,
    kGuestNice_
  };

  enum ProcessStates
  {
    kUserTime_ = 14,
    kSysTime_ = 15,
    kChildUserTime_ = 16,
    kChildSysTime_ = 17
  };

  std::vector<float> CpuUtilization(const std::string &cpu_id);
  std::vector<long> CpuData(const std::string &cpu_id);
  long ActiveJiffies(const std::string &cpu_id);
  long ActiveJiffies(int pid);
  long IdleJiffies(const std::string &cpu_id);

  // Processes
  std::string Command(int pid);
  std::string Ram(int pid);
  std::string Uid(int pid);
  std::string User(int pid);
  long int UpTime(int pid);
  float CpuUtilization(int pid);

  template <typename T>
  T ParseFileForKey(const std::string &path, const std::string &key);
}; // namespace LinuxParser

#endif