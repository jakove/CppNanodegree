#include "processor.h"

#include <fstream>

#include "linux_parser.h"

Processor::Processor(Mode mode) : mode_(mode) { GetCpus(); }

const std::vector<std::string> &Processor::CpuIds() { return cpu_ids_; }

void Processor::SetMode(Mode mode)
{
  mode_ = mode;
  GetCpus();
}

std::string Processor::ConnvertIdTOName(const std::string &cpu_id)
{
  std::string name{cpu_id};
  name.replace(0, 1, "C");
  return name;
}

float Processor::Utilization(const std::string &cpu_id)
{
  float idle = static_cast<float>(LinuxParser::IdleJiffies(cpu_id));
  float active = static_cast<float>(LinuxParser::ActiveJiffies(cpu_id));
  float sum = idle + active;
  float delta_idle = idle - previous_idle_[cpu_id];
  float delta_sum = sum - previous_sum_[cpu_id];
  float utilization = (delta_sum - delta_idle) / delta_sum;

  previous_sum_[cpu_id] = sum;
  previous_idle_[cpu_id] = idle;

  return utilization;
}

void Processor::GetCpus()
{
  std::ifstream stat(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
  cpu_ids_.clear();
  if (stat.is_open())
  {
    std::string line;
    while (std::getline(stat, line))
    {
      std::istringstream linestream(line);
      std::string value;
      linestream >> value;

      // if Mode == info check only for "cpu"
      // otherwise check for all cpus except for "cpu"
      if (value.find("cpu") != std::string::npos)
      {
        if (((mode_ == kStandard) && (value == "cpu")) ||
            (mode_ == kInfo && (value != "cpu")))
        {
          cpu_ids_.push_back(value);
          previous_idle_[value] = 0;
          previous_sum_[value] = 0;
        }
      }
    }
  }
}