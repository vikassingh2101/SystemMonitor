#include "processor.h"
#include "linux_parser.h"

float Processor::Utilization() {
    long active_jiffies = LinuxParser::ActiveJiffies();
    long idle_jiffies = LinuxParser::IdleJiffies();
    long total_jiffies = LinuxParser::Jiffies();
    long prev_total_jiffies = prev_active_jiffies + prev_idle_jiffies;
    long total_diff = total_jiffies - prev_total_jiffies;
    long idle_diff = idle_jiffies - prev_idle_jiffies;

    prev_active_jiffies = active_jiffies;
    prev_idle_jiffies = idle_jiffies;
    
    return (total_diff - idle_diff)*1.0/total_diff;
}