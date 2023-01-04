#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  int Pid();                               // TODO: See src/process.cpp
  std::string User() const;                // TODO: See src/process.cpp
  std::string Command() const;             // TODO: See src/process.cpp
  float CpuUtilization() const;            // TODO: See src/process.cpp
  std::string Ram() const;                 // TODO: See src/process.cpp
  long int UpTime() const;                 // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp
  bool operator>(Process const& a) const;
  void Update();
  bool HasUpdated();

  // TODO: Declare any necessary private members
 private:
  int pid_ = 0;
  std::string user_ = "";
  std::string command_ = "";
  long int uptime_ = 0;
  double cpu_utilization_ = 0.0;
  bool updated_ = true;
  std::string ram_ = "";
  long active_jiffies_ = 0;
  long total_jiffies_ = 0;

  double CalculateCpuUtilization();
};

#endif