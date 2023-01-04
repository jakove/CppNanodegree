#include <dirent.h>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

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
  string os, kernel, version;
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
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string memfile_path = kProcDirectory + kMeminfoFilename;
  auto mem_total =
      LinuxParser::ParseFileForKey<float>(memfile_path, "MemTotal:");
  auto mem_free =
      LinuxParser::ParseFileForKey<float>(memfile_path, "MemAvailable:");
  return (mem_total - mem_free) / mem_total;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream uptime_file(kProcDirectory + kUptimeFilename);
  std::string line;
  // initialize -1 to indicate an error if the uptime couldn't be read.
  float uptime = -1, idletime = -1;
  if (uptime_file.is_open()) {
    std::getline(uptime_file, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::string path = kProcDirectory + std::to_string(pid) + "/stat";
  std::ifstream stat_file(path);
  if (stat_file.is_open()) {
    std::string line;
    float utime, stime, cutime, cstime;
    if (std::getline(stat_file, line)) {
      std::istringstream linestream(line);
      std::string value;
      for (int idx = 1; linestream >> value; idx++) {
        switch (idx) {
          case kUserTime_:
            utime = stof(value);
            break;
          case kSysTime_:
            stime = stof(value);
            break;
          case kChildUserTime_:
            cutime = stof(value);
            break;
          case kChildSysTime_:
            cstime = stof(value);
            break;
        }
        if (idx > kChildSysTime_) {
          break;
        }
      }
      long total_time = (utime + stime + cutime + cstime);
      return total_time;
    }
  }
  return -1;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies(const std::string& cpu_id) { 
  auto cpu_data = LinuxParser::CpuUtilization(cpu_id);
  return cpu_data[kUser_] + cpu_data[kNice_] + cpu_data[kSystem_] + 
         cpu_data[kIRQ_] + cpu_data[kSoftIRQ_] + cpu_data[kSteal_];;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies(const std::string& cpu_id) {
  auto cpu_data = LinuxParser::CpuUtilization(cpu_id);
  return cpu_data[kIdle_] + cpu_data[kIOwait_];
}

vector<long> LinuxParser::CpuData(const std::string& cpu_id) {
  std::ifstream stat_file(kProcDirectory + kStatFilename);
  std::vector<long> cpu_data{};
  if (stat_file.is_open()) {
    std::string line;
    while (std::getline(stat_file, line)) {
      std::stringstream linestream(line);
      std::string value;
      linestream >> value;
      if (value == cpu_id) {
        while (linestream >> value) {
          cpu_data.push_back(std::stof(value));
        }
        return cpu_data;
      }
    }
  }
  return cpu_data;
}

// TODO: Read and return CPU utilization
vector<float> LinuxParser::CpuUtilization(const std::string& cpu_id) {
  std::ifstream stat_file(kProcDirectory + kStatFilename);
  std::vector<float> cpu_data{};
  if (stat_file.is_open()) {
    std::string line;
    while (std::getline(stat_file, line)) {
      std::stringstream linestream(line);
      std::string value;
      linestream >> value;
      if (value == cpu_id) {
        while (linestream >> value) {
          cpu_data.push_back(std::stof(value));
        }
        return cpu_data;
      }
    }
  }
  return cpu_data;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string stat_path = kProcDirectory + kStatFilename;
  auto total_processes =
      LinuxParser::ParseFileForKey<int>(stat_path, "processes");
  return total_processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string stat_path = kProcDirectory + kStatFilename;
  auto total_processes =
      LinuxParser::ParseFileForKey<int>(stat_path, "procs_running");
  return total_processes;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::string path = kProcDirectory + std::to_string(pid) + "/cmdline";
  std::ifstream cmd_file(path);
  if (cmd_file.is_open()) {
    std::string line;
    if (std::getline(cmd_file, line)) {
      std::stringstream linestream(line);
      std::string cmd;
      linestream >> cmd;
      return cmd;
    }
  }
  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  std::string path = kProcDirectory + to_string(pid) + kStatusFilename;
  double memory = ParseFileForKey<double>(path, "VmSize:");
  memory *= 0.0009765625;
  std::stringstream memory_ss;
  memory_ss << std::fixed << std::setprecision(1) << memory;
  return memory_ss.str();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::string file_path = kProcDirectory + std::to_string(pid) + "/status";
  std::string uid = ParseFileForKey<std::string>(file_path, "Uid:");
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  auto uid = Uid(pid);
  std::ifstream pw_file(kPasswordPath);
  if (pw_file.is_open()) {
    std::string line;
    while (std::getline(pw_file, line)) {
      auto pos = line.find(uid + ":" + uid);
      if (pos != std::string::npos) {
        return line.substr(0, pos - 3);
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  std::string path = kProcDirectory + std::to_string(pid) + "/stat";
  std::ifstream stat_file(path);
  if (stat_file.is_open()) {
    std::string line;
    if (std::getline(stat_file, line)) {
      std::istringstream linestream(line);
      int start_time_position = 22;
      std::string start_time;
      for (int idx = 1; idx <= start_time_position; idx++) {
        if (!(linestream >> start_time)) {
          return -1;
        }
      };

      return stol(start_time) / sysconf(_SC_CLK_TCK);
    }
  }
  return -1;
}

template <typename T>
T LinuxParser::ParseFileForKey(const std::string& path,
                               const std::string& key) {
  std::ifstream file(path);
  std::string line;
  T value{};
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream linestream(line);
      string file_key{};
      linestream >> file_key;
      if (file_key == key) {
        linestream >> value;
        return value;
      }
    }
  }
  return value;
}