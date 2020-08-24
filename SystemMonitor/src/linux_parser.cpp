#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <algorithm>
#include <string>
#include <type_traits>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

std::vector<std::string> LinuxParser::ParseProcessFile(
    const std::string& abs_path, const std::string& lookup_token,
    char delimiter, int row, std::vector<int>& columns) {
  std::vector<std::string> parsed_information;
  std::string line{};
  std::string parsed_token{};
  std::ifstream filestream(abs_path);
  int row_count{};
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (!lookup_token.empty()) {  // row is selected based on lookup token
        std::replace(line.begin(), line.end(), delimiter, ' ');
        std::istringstream linestream(line);
        linestream >> parsed_token;
        if (parsed_token == lookup_token) {
          return LinuxParser::ParseLine(line, columns);
        }       // end if
      } else {  // row is selected based on row number
        if (row == row_count++) {
          std::replace(line.begin(), line.end(), delimiter, ' ');
          return LinuxParser::ParseLine(line, columns);
        }
      }  // else
    }    // end while
  }      // end if
  return {};
}

std::vector<std::string> LinuxParser::ParseLine(const std::string& line,
                                                std::vector<int>& columns) {
  std::string parsed_token{};
  std::istringstream linestream(line);
  int actual_column{0};
  std::size_t desired_column_index{};
  std::vector<std::string> parsed_information;
  while (linestream >> parsed_token) {
    if (columns[desired_column_index] == actual_column++) {
      parsed_information.push_back(parsed_token);
      desired_column_index++;
      if (desired_column_index > columns.size() - 1) {
        return parsed_information;
      }  // end if
    }    // end if
  }      // end while
  return {};
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line{};
  string key{};
  string value{};
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
  std::vector<std::string> parsed_information;
  std::vector<int> columns_of_interest{2};
  parsed_information = LinuxParser::ParseProcessFile(
      kProcDirectory + kVersionFilename, {}, ' ', 0, columns_of_interest);
  if (!parsed_information.empty()) {
    return parsed_information[0];
  } else {
    return {};
  }
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids{};
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
  std::vector<std::string> parsed_information;
  std::vector<int> columns_of_interest{1};
  parsed_information =
      LinuxParser::ParseProcessFile(kProcDirectory + kMeminfoFilename,
                                    "MemTotal:", ' ', 0, columns_of_interest);
  if (!parsed_information.empty()) {
    mem_total = std::stof(parsed_information[0]);
  }

  parsed_information.clear();

  parsed_information =
      LinuxParser::ParseProcessFile(kProcDirectory + kMeminfoFilename,
                                    "MemFree:", ' ', 0, columns_of_interest);
  if (!parsed_information.empty()) {
    mem_free = std::stof(parsed_information[0]);
  }

  if (almost_equal(mem_total, 0.0f, 2)) {
    return 0.0f;
  } else {
    return (mem_total - mem_free) / mem_total;
  }
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  std::vector<std::string> parsed_information;
  std::vector<int> columns_of_interest{0};
  parsed_information = LinuxParser::ParseProcessFile(
      kProcDirectory + kUptimeFilename, {}, ' ', 0, columns_of_interest);
  if (!parsed_information.empty()) {
    return stol(parsed_information[0]);
  } else {
    return 0;
  }
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long jiffies_total{};
  std::vector<std::string> parsed_cpu_numbers = LinuxParser::CpuTimeParser();
  for (const std::string& jiffy : parsed_cpu_numbers) {
    jiffies_total += std::stol(jiffy);
  }
  return jiffies_total;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::vector<std::string> parsed_process_status =
      LinuxParser::ProcessStatus(pid);

  if (!parsed_process_status.empty()) {
    // assign currently parsed values
    long utime{std::stol(parsed_process_status[static_cast<int>(
        LinuxParser::PROCESS_STAT::utime)])};
    long stime{std::stol(parsed_process_status[static_cast<int>(
        LinuxParser::PROCESS_STAT::stime)])};
    long cutime{std::stol(parsed_process_status[static_cast<int>(
        LinuxParser::PROCESS_STAT::cutime)])};
    long cstime{std::stol(parsed_process_status[static_cast<int>(
        LinuxParser::PROCESS_STAT::cstime)])};

    // Calculation
    return utime + stime + cutime + cstime;
  }
  return 0;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long number_active_jiffies{};
  std::vector<std::string> parsed_cpu_numbers = LinuxParser::CpuTimeParser();
  if (!parsed_cpu_numbers.empty()) {
    number_active_jiffies += std::stol(parsed_cpu_numbers[static_cast<int>(
                                 LinuxParser::CPU_STATES::user)]) +
                             std::stol(parsed_cpu_numbers[static_cast<int>(
                                 LinuxParser::CPU_STATES::nice)]) +
                             std::stol(parsed_cpu_numbers[static_cast<int>(
                                 LinuxParser::CPU_STATES::system)]) +
                             std::stol(parsed_cpu_numbers[static_cast<int>(
                                 LinuxParser::CPU_STATES::irq)]) +
                             std::stol(parsed_cpu_numbers[static_cast<int>(
                                 LinuxParser::CPU_STATES::soft_irq)]) +
                             std::stol(parsed_cpu_numbers[static_cast<int>(
                                 LinuxParser::CPU_STATES::steal)]) +
                             std::stol(parsed_cpu_numbers[static_cast<int>(
                                 LinuxParser::CPU_STATES::guest)]) +
                             std::stol(parsed_cpu_numbers[static_cast<int>(
                                 LinuxParser::CPU_STATES::guest_nice)]);
  }
  return number_active_jiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long number_idle_jiffies{};
  std::vector<std::string> parsed_cpu_numbers = LinuxParser::CpuTimeParser();
  if (!parsed_cpu_numbers.empty()) {
    number_idle_jiffies += std::stol(parsed_cpu_numbers[static_cast<int>(
                               LinuxParser::CPU_STATES::idle)]) +
                           std::stol(parsed_cpu_numbers[static_cast<int>(
                               LinuxParser::CPU_STATES::io_wait)]);
  }
  return number_idle_jiffies;
}

// Read and return CPU utilization
vector<std::string> LinuxParser::CpuTimeParser() {
  std::vector<std::string> relevant_numbers;
  std::vector<std::string> parsed_information;
  std::vector<int> columns_of_interest{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  parsed_information = LinuxParser::ParseProcessFile(
      kProcDirectory + kStatFilename, "cpu", ' ', 0, columns_of_interest);

  relevant_numbers.reserve(parsed_information.size());
  for (const std::string& elem : parsed_information) {
    relevant_numbers.push_back(elem);
  }

  return relevant_numbers;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::vector<std::string> parsed_information;
  std::vector<int> columns_of_interest{1};
  parsed_information = LinuxParser::ParseProcessFile(
      kProcDirectory + kStatFilename, "processes", ' ', 0, columns_of_interest);

  if (!parsed_information.empty()) {
    return std::stoi(parsed_information[0]);
  } else {
    return 0;
  }
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::vector<std::string> parsed_information;
  std::vector<int> columns_of_interest{1};
  parsed_information = LinuxParser::ParseProcessFile(
      kProcDirectory + kStatFilename, "procs_running", ' ', 0,
      columns_of_interest);
  if (!parsed_information.empty()) {
    return std::stoi(parsed_information[0]);
  } else {
    return 0;
  }
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::vector<std::string> parsed_information;
  std::vector<int> columns_of_interest{0};
  parsed_information = LinuxParser::ParseProcessFile(
      kProcDirectory + std::to_string(pid) + kCmdlineFilename, {}, ' ', 0,
      columns_of_interest);
  if (!parsed_information.empty()) {
    return parsed_information[0];
  } else {
    return {};
  }
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::vector<std::string> parsed_information;
  std::vector<int> columns_of_interest{1};
  parsed_information = LinuxParser::ParseProcessFile(
      kProcDirectory + std::to_string(pid) + kStatusFilename, "VmSize:", ' ', 0,
      columns_of_interest);
  if (!parsed_information.empty()) {
    return parsed_information[0];
  } else {
    return {};
  }
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::vector<std::string> parsed_information;
  std::vector<int> columns_of_interest{1};
  parsed_information = LinuxParser::ParseProcessFile(
      kProcDirectory + std::to_string(pid) + kStatusFilename, "Uid:", ' ', 0,
      columns_of_interest);
  if (!parsed_information.empty()) {
    return parsed_information[0];
  } else {
    return {};
  }
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line{};
  string user{};
  string temp{};
  string uid{};
  string lookup_uid{LinuxParser::Uid(pid)};
  std::ifstream filestream(kPasswordPath);

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

// Read and return selected status information of the corresponding process
vector<std::string> LinuxParser::ProcessStatus(int pid) {
  std::vector<int> columns_of_interest{2, 13, 14, 15, 16, 21};
  return LinuxParser::ParseProcessFile(
      kProcDirectory + std::to_string(pid) + kStatFilename, {}, ' ', 0,
      columns_of_interest);
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::vector<std::string> parsed_information;
  std::vector<int> columns_of_interest{21};
  parsed_information = LinuxParser::ParseProcessFile(
      kProcDirectory + std::to_string(pid) + kStatFilename, {}, ' ', 0,
      columns_of_interest);

  if (!parsed_information.empty()) {
    return LinuxParser::UpTime() -
           (stoll(parsed_information[0])) /
               static_cast<long long>(sysconf(_SC_CLK_TCK));
  } else {
    return 0;
  }
}

char LinuxParser::ProcessState(int pid) {
  std::vector<std::string> parsed_process_status =
      LinuxParser::ProcessStatus(pid);
  if (!parsed_process_status.empty()) {
    std::string retVal{parsed_process_status[static_cast<int>(
        LinuxParser::PROCESS_STAT::state)]};
    return retVal[0];
  }
  return ' ';
}