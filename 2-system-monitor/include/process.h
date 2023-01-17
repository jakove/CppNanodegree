#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process
{
public:
  Process(int pid);
  int Pid();
  std::string User() const;
  std::string Command() const;
  float CpuUtilization() const;
  std::string Ram() const;
  long int UpTime() const;
  bool operator<(Process const &a) const;
  bool operator>(Process const &a) const;
  void Update();
  bool HasUpdated();

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