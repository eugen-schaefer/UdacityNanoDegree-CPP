#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

  // Declare any necessary private members
 private:
  long previous_idle_time{};
  long previous_non_idle_time{};
  long current_idle_time{};
  long current_non_idle_time{};
};

#endif