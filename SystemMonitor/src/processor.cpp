#include "processor.h"

#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
/*
 * calculation source:
 * https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
 * */
float Processor::Utilization() {
  std::vector<std::string> parsed_cpu_numbers = LinuxParser::CpuUtilization();

  if (!parsed_cpu_numbers.empty()){
    // old values from last cycle are the previous ones
    float previous_idle_time{idle + iowait};
    float previous_non_idle_time{user + nice + system + irq + softirq + steal};

    // assign currently parsed values
    user = std::stof(parsed_cpu_numbers[LinuxParser::kUser_]);
    nice = std::stof(parsed_cpu_numbers[LinuxParser::kNice_]);
    system = std::stof(parsed_cpu_numbers[LinuxParser::kSystem_]);
    idle = std::stof(parsed_cpu_numbers[LinuxParser::kIdle_]);
    iowait = std::stof(parsed_cpu_numbers[LinuxParser::kIOwait_]);
    irq = std::stof(parsed_cpu_numbers[LinuxParser::kIRQ_]);
    softirq = std::stof(parsed_cpu_numbers[LinuxParser::kSoftIRQ_]);
    steal = std::stof(parsed_cpu_numbers[LinuxParser::kSteal_]);
    guest = std::stof(parsed_cpu_numbers[LinuxParser::kGuest_]);
    guest_nice = std::stof(parsed_cpu_numbers[LinuxParser::kGuestNice_]);

    // new assigned values are the current ones
    float current_idle_time{idle + iowait};
    float current_non_idle_time{user + nice + system + irq + softirq + steal};

    float previous_total_time{previous_idle_time + previous_non_idle_time};
    float current_total_time{current_idle_time + current_non_idle_time};

    float total_difference{current_total_time - previous_total_time};
    float idle_difference{current_idle_time - previous_idle_time};

    return (total_difference - idle_difference)/total_difference;
  }else
  {
    return 0.0f;
  }
}