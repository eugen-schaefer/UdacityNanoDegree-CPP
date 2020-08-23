#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  explicit Process(int pid) : m_pid{pid} { UpdateProcessInformation(); }
  int Pid() const;
  std::string User() const;
  std::string Command() const;
  float CpuUtilization() const;
  std::string Ram() const;
  long int UpTime() const;
  bool operator<(Process const& a) const;
  void UpdateProcessInformation();

 private:
  int m_pid{};
  long m_previous_jiffies{};
  long m_current_jiffies{};
  long m_previous_starttime{};
  long m_current_starttime{};
  float m_virtual_memory_size_mb{};
  std::string m_user{};
  std::string m_command{};
};

#endif
