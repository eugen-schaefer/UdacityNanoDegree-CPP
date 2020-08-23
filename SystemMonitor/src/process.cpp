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

void Process::UpdateProcessInformation() {
  // Jiffies
  m_previous_jiffies = m_current_jiffies;
  m_previous_starttime = m_current_starttime;
  m_current_jiffies = LinuxParser::ActiveJiffies(Pid());
  m_current_starttime = LinuxParser::UpTime(Pid());

  // Process command
  m_command = LinuxParser::Command(Pid());

  // RAM
  std::string memory_str_KB{LinuxParser::Ram(Pid())};
  if (!memory_str_KB.empty()) {
    m_virtual_memory_size_mb = std::stof(memory_str_KB) / 1000;
  }

  // User
  m_user = LinuxParser::User(Pid());
}

// Return this process's ID
int Process::Pid() const { return m_pid; }

// Return this process's CPU utilization
/*
 * calculation source:
 * https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
 * */
float Process::CpuUtilization() const {
  long jiffies_difference = m_current_jiffies - m_previous_jiffies;
  long uptime_difference = m_current_starttime - m_previous_starttime;

  if (jiffies_difference <= 0 || uptime_difference <= 0) {
    return 0.0f;
  } else {
    return (static_cast<float>(jiffies_difference) /
            static_cast<float>(sysconf(_SC_CLK_TCK))) /
           static_cast<float>(uptime_difference);
  }
}

// Return the command that generated this process
string Process::Command() const { return m_command; }

// Return this process's memory utilization
string Process::Ram() const {
  int precision = 2;
  return std::to_string(m_virtual_memory_size_mb)
      .substr(0, std::to_string(m_virtual_memory_size_mb).find(".") +
                     precision + 1);
}

// Return the user (name) that generated this process
string Process::User() const { return m_user; }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return m_current_starttime; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return (this->CpuUtilization() > a.CpuUtilization());
  // return (this->UpTime() > a.UpTime());
  // return (std::stof(this->Ram()) > std::stof(a.Ram()));
}
