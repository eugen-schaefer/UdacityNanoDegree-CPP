#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid):pid_{pid}{}
  int Pid() const;
  std::string User() const;
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime() const;
  bool operator<(Process& a);  // TODO: Consider to make const, const (bool operator<(Process const& a) const;)

 private:
  int pid_{};
  long previous_process_jiffies{};
  long current_process_jiffies{};
  long previous_process_uptime{};
  long current_process_uptime{};
};

#endif
