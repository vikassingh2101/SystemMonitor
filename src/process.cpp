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
    long uptime = LinuxParser::UpTime();
    long active_jiffies = LinuxParser::ActiveJiffies(pid_);
    float seconds = uptime - this->UpTime();
    float cpu_usage = 100.0*active_jiffies/sysconf(_SC_CLK_TCK)/seconds;
    return cpu_usage;
}

string Process::Command() const { return LinuxParser::Command(pid_); }

string Process::Ram() const { return to_string( stol(LinuxParser::Ram(pid_)) / 1024 ); }

string Process::User() const { return LinuxParser::User(pid_); }

long int Process::UpTime() const { return LinuxParser::UpTime(pid_)/sysconf(_SC_CLK_TCK); }

bool Process::operator<(Process const& a) const{
    return this->CpuUtilization() < a.CpuUtilization();
    //return stof(this->Ram()) < stof(a.Ram());
}