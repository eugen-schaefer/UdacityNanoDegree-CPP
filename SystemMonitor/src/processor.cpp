#include "processor.h"

#include "linux_parser.h"

// Return the aggregate CPU utilization
/*
 * calculation source:
 * https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
 * */
float Processor::Utilization() {
  // old values from last cycle are the previous ones
  previous_non_idle_time = current_non_idle_time;
  previous_idle_time = current_idle_time;

  // assign currently parsed values
  current_non_idle_time = LinuxParser::ActiveJiffies();
  current_idle_time = LinuxParser::IdleJiffies();

  // calculation
  long previous_total_time{previous_idle_time + previous_non_idle_time};
  long current_total_time{current_idle_time + current_non_idle_time};

  long total_difference{current_total_time - previous_total_time};
  long idle_difference{current_idle_time - previous_idle_time};

  return static_cast<float>((total_difference - idle_difference)) /
         static_cast<float>(total_difference);
}