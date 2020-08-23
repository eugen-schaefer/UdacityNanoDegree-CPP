#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  // clear all pids which were active in the previous cycle
  pids_.clear();

  // read all active pids
  pids_ = LinuxParser::Pids();

  // In case the process container does not contain the active pid, create a new
  // process with the active pid and insert it into the process container
  for (auto active_pid : pids_) {
    bool is_active_pid_already_tracked{
        std::any_of(processes_.begin(), processes_.end(),
                    [active_pid](const Process& proc) {
                      return proc.Pid() == active_pid;
                    })};
    if (!is_active_pid_already_tracked) {
      Process process(active_pid);
      char state = LinuxParser::ProcessState(active_pid);
      // Don't insert processes with states "Zombie", "Stopped", "Dead"
      if (state != 'Z' && state != 'T' && state != 't' && state != 'X' &&
          state != 'x' && !process.Command().empty()) {
        processes_.push_back(process);
      }
    }
  }

  // Erase all entries in the process container which do not reflect the active
  // pids
  for (auto proc_iter = processes_.begin(); proc_iter != processes_.end();) {
    bool is_process_pid_still_active{std::any_of(
        pids_.begin(), pids_.end(),
        [proc_iter](int pid) { return pid == (*proc_iter).Pid(); })};

    if (is_process_pid_still_active) {
      ++proc_iter;
    } else {
      proc_iter = processes_.erase(proc_iter);
    }
  }

  // sort the process container
  std::sort(processes_.begin(), processes_.end(), [](Process a, Process b){return a < b;});

  return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
