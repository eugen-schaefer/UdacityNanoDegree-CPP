#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  float user{};
  float nice{};
  float system{};
  float idle{};
  float iowait{};
  float irq{};
  float softirq{};
  float steal{};
  float guest{};
  float guest_nice{};
};

#endif