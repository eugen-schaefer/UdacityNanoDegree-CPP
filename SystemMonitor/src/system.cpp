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
Processor& System::Cpu() { return m_cpu; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  // clear all pids which were active in the previous cycle
  m_pids.clear();

  // read all active pids
  m_pids = LinuxParser::Pids();

  // In case the process container does not contain the active pid, create a new
  // process with the active pid and insert it into the process container
  for (auto active_pid : m_pids) {
    bool is_active_pid_already_tracked{
        std::any_of(m_processes.begin(), m_processes.end(),
                    [active_pid](const Process& proc) {
                      return proc.Pid() == active_pid;
                    })};
    if (!is_active_pid_already_tracked) {
      Process process(active_pid);
      char state = LinuxParser::ProcessState(active_pid);
      // Don't insert processes with states "Zombie", "Stopped", "Dead"
      if (state != 'Z' && state != 'T' && state != 't' && state != 'X' &&
          state != 'x') {
        m_processes.push_back(process);
      }
    }
  }

  // Erase all entries in the process container which do not reflect the active
  // pids
  for (auto proc_iter = m_processes.begin(); proc_iter != m_processes.end();) {
    bool is_process_pid_still_active{std::any_of(
        m_pids.begin(), m_pids.end(),
        [proc_iter](int pid) { return pid == (*proc_iter).Pid(); })};

    if (is_process_pid_still_active) {
      // update process states
      // this approach updating the process states here helps avoid
      // the circumstances during sorting where some pids might be not valid
      // anymore
      proc_iter->UpdateProcessInformation();
      ++proc_iter;
    } else {
      proc_iter = m_processes.erase(proc_iter);
    }
  }

  // sort the process container
  std::sort(m_processes.begin(), m_processes.end(),
            [](const Process& a, const Process& b) { return a < b; });

  return m_processes;
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
