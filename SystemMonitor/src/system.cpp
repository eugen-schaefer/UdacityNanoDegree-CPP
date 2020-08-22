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

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  processes_.clear();
  pids_.clear();

  // read all active pids
 //pids_ = LinuxParser::Pids();
  pids_.push_back(8956);

  // create a process instance for every pid and put it into processes_
  for (int elem : pids_){
    Process process(elem);
    // ignore zombie processes
    if (!process.Command().empty()){
      processes_.push_back(process);
    }
  }

  return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return 0; }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return 0; }

// Return the number of seconds since the system started running
long int System::UpTime() {
  return LinuxParser::UpTime();
}
