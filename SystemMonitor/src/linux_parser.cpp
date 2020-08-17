#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float mem_total{};
  float mem_free{};
  float mem_used{};
  string line;
  int number_parsed_lines{};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      string description_string, integer_string, unit_string;
      std::istringstream linestream(line);
      while (linestream >> description_string >> integer_string >>
             unit_string) {
        if (description_string == "MemTotal:") {
          mem_total = std::stof(integer_string);
          ++number_parsed_lines;
        }
        if (description_string == "MemFree:") {
          mem_free = std::stof(integer_string);
          ++number_parsed_lines;
        }
      }
      if (number_parsed_lines == 2) {
        mem_used = (mem_total - mem_free) / mem_total;
        break;
      }
    }
  }

  return mem_used;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    string system_uptime;
    std::istringstream linestream(line);
    linestream >> system_uptime;
    return stol(system_uptime);
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// Read and return CPU utilization
vector<std::string> LinuxParser::CpuUtilization() {
  std::vector<std::string> relevant_numbers(10);
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      string description_string;
      std::istringstream linestream(line);
      while (linestream >> description_string) {
        if (description_string == "cpu") {
          linestream >> relevant_numbers[kUser_] >> relevant_numbers[kNice_] >>
              relevant_numbers[kSystem_] >> relevant_numbers[kIdle_] >>
              relevant_numbers[kIOwait_] >> relevant_numbers[kIRQ_] >>
              relevant_numbers[kSoftIRQ_] >> relevant_numbers[kSteal_] >>
              relevant_numbers[kGuest_] >> relevant_numbers[kGuestNice_];
          return relevant_numbers;
        }
      }
    }
  }
  return {};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return 0; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { return 0; }

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line{};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line{};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      string description_string{};
      std::istringstream linestream(line);
      while (linestream >> description_string) {
        if (description_string == "VmSize:") {
          string memory{};
          linestream >> memory;
          return memory;
        }
      }
    }
  }
  return string();
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line{};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  // TODO: Make a read function out of it
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      string description_string{};
      std::istringstream linestream(line);
      while (linestream >> description_string) {
        if (description_string == "Uid:") {
          string uid{};
          linestream >> uid;
          return uid;
        }
      }
    }
  }
  return string();
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line{};
  string user{};
  string temp{};
  string uid{};
  string lookup_uid{LinuxParser::Uid(pid)};
  std::ifstream filestream(kPasswordPath);
  // TODO: Make a read function out of it
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> temp >> uid;
      if (uid == lookup_uid) {
        return user;
      }
    }
  }
  return string();
}

float LinuxParser::CpuUtilizationPerProcess(int pid) { return 0.0f; }

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    string process_uptime;
    std::istringstream linestream(line);
    for (int idx = 0; idx < 22; ++idx) {
      linestream >> process_uptime;
    }

    return LinuxParser::UpTime() -
           (stol(process_uptime)) / (sysconf(_SC_CLK_TCK));
  }

  return 0;
}
