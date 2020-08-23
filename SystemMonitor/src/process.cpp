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
 * https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
 * */
float Process::CpuUtilization() {
  // old values from last cycle are the previous ones
  previous_process_jiffies = current_process_jiffies;
  previous_process_uptime = current_process_uptime;

  // assign currently parsed values
  std::vector<std::string> parsed_process_status =
      LinuxParser::ProcessStatus(Pid());
  int starttime_index = static_cast<int>(LinuxParser::PROCESS_STAT::starttime);
  long starttime = std::stol(parsed_process_status[starttime_index]);

  current_process_jiffies = LinuxParser::ActiveJiffies(Pid());
  current_process_uptime =
      UpTime() - starttime / static_cast<long>(sysconf(_SC_CLK_TCK));

  long jiffies_difference = current_process_jiffies - previous_process_jiffies;
  long uptime_difference = current_process_uptime - previous_process_uptime;

  if (uptime_difference == 0) {
    return 0.0f;
  } else {
    return (static_cast<float>(jiffies_difference) /
            static_cast<float>(sysconf(_SC_CLK_TCK))) /
           static_cast<float>(uptime_difference);
  }
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(Pid()); }

// Return this process's memory utilization
string Process::Ram() {
  std::string memory_str_KB{LinuxParser::Ram(Pid())};
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
string Process::User() const { return LinuxParser::User(Pid()); }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(Pid()); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process& a) {
  return (this->CpuUtilization() > a.CpuUtilization());
  // return (this->UpTime() > a.UpTime());
  // return (std::stof(this->Ram()) > std::stof(a.Ram()));
}
