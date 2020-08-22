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

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
/*
 * calculation source:
 * https://stackoverflow.com/questions/1420426/how-to-calculate-the-cpu-usage-of-a-process-by-pid-in-linux-from-c/1424556#1424556
 * */
float Process::CpuUtilization() {
  // old values from last cycle are the previous ones
  previous_process_jiffies = current_process_jiffies;
  previous_total_cpu_jiffies = current_total_cpu_jeffies;

  // assign currently parsed values
  current_process_jiffies = LinuxParser::ActiveJiffies(pid_);
  current_total_cpu_jeffies = LinuxParser::Jiffies();

  float retVal =
      static_cast<float>((current_process_jiffies - previous_process_jiffies)) /
      static_cast<float>(
          (current_total_cpu_jeffies - previous_total_cpu_jiffies));

  return retVal;
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() {
  std::string memory_str_KB{LinuxParser::Ram(pid_)};
  if (!memory_str_KB.empty()) {
    float memory_float_MB{std::stof(memory_str_KB) / 1000};
    int precision = 2;
    return std::to_string(memory_float_MB)
        .substr(0, std::to_string(memory_float_MB).find(".") + precision + 1);
  } else {
    return "0.0";
  }
}

// Return the user (name) that generated this process
string Process::User() const { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a [[maybe_unused]]) const {
  return true;
}
