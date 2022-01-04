#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const {
    long active_jiffies = LinuxParser::ActiveJiffies(pid_);
    float process_uptime = this->UpTime();
    float cpu_usage = 100.0*active_jiffies/sysconf(_SC_CLK_TCK)/process_uptime;
    return cpu_usage;
}

string Process::Command() const { return LinuxParser::Command(pid_); }

string Process::Ram() const { return to_string( stol(LinuxParser::Ram(pid_)) / 1024 ); }

string Process::User() const { return LinuxParser::User(pid_); }

long int Process::UpTime() const {
    //Please find the updated code for retrieving the uptime of the given process. This code change has been done based on inputs given by reviewer.
    long uptime = LinuxParser::UpTime();
    long starttime = LinuxParser::UpTime(pid_)/sysconf(_SC_CLK_TCK);
    return uptime-starttime;
}

bool Process::operator<(Process const& a) const{
    return this->CpuUtilization() < a.CpuUtilization();
    //return stof(this->Ram()) < stof(a.Ram());
}