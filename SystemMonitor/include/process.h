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
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

  // TODO: Declare any necessary private members
 private:
  int pid_{};
};

#endif
