#include "format.h"

#include <string>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  string hh = std::to_string(seconds / 3600);
  string mm = std::to_string(seconds / 60 % 60);
  string ss = std::to_string(seconds % 60);

  hh = AddLeadingZeros(hh);
  mm = AddLeadingZeros(mm);
  ss = AddLeadingZeros(ss);

  return hh + ":" + mm + ":" + ss;
}

std::string Format::AddLeadingZeros(const std::string& time_slice) {
  std::string converted_slice = time_slice;
  if (time_slice.size() == 1) {
    converted_slice = "0" + time_slice;
  }
  return converted_slice;
};