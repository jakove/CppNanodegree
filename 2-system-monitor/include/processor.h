#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <map>
#include <string>
#include <vector>

class Processor {
 public:

  // Mode is used to change the information shown on terminal.
  enum Mode {
    kStandard, // Informationmode as given in the task: List only single CPU information
    kInfo, // Is used to list each single cpu core
  };

  Processor(Mode mode); 
  
  float Utilization(const std::string& cpu_id);  // TODO: See src/processor.cpp
  
  // Returns all found Cpu Ids
  const std::vector<std::string>& CpuIds();
  
  // Replacing small c by a capital c for nicer user-output
  std::string ConnvertIdTOName (const std::string& cpu_id);
  
  // Change mode to get another information level
  void SetMode(Mode mode);

 private:
 
  std::map<std::string, float> previous_sum_; 
  std::map<std::string, float> previous_idle_;
  std::vector<std::string> cpu_ids_;
  Mode mode_ = Mode::kStandard;

  void GetCpus();
};

#endif