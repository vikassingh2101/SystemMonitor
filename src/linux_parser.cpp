#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;


string LinuxParser::OperatingSystem() {
  string line, key, value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "PRETTY_NAME") {
        std::replace(value.begin(), value.end(), '_', ' ');
        return value;
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  int memTotal = 0;
  int memFree = 0;
  string line, temp;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      linestream >> temp >> memTotal;
    }
    {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      linestream >> temp >> memFree;
    }
  }
  return (memTotal-memFree)*1.0f/memTotal;
}

long LinuxParser::UpTime() { 
  string uptime{"0"};
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return long(stof(uptime));
}

long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + "/" + kStatFilename);
  long active_jiffies = 0;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    int count = 0;
    while( (linestream >> value) && ++count <= 13 );
    active_jiffies += stol(value);
    linestream >> value;
    active_jiffies += stol(value);
    linestream >> value;
    active_jiffies += stol(value);
    linestream >> value;
    active_jiffies += stol(value);
    return active_jiffies;
  }
  return 0;
}

long LinuxParser::ActiveJiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  long active_jiffies = stol(jiffies[CPUStates::kUser_]) +
                        stol(jiffies[CPUStates::kNice_]) +
                        stol(jiffies[CPUStates::kSystem_]) +
                        stol(jiffies[CPUStates::kIRQ_]) +
                        stol(jiffies[CPUStates::kSoftIRQ_]) +
                        stol(jiffies[CPUStates::kSteal_]);
  return active_jiffies;
}

long LinuxParser::IdleJiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  long idle_jiffies = stol(jiffies[CPUStates::kIdle_]) +
                      stol(jiffies[CPUStates::kIOwait_]);
  return idle_jiffies;
}

vector<string> LinuxParser::CpuUtilization() {
  string line, key, value;
  vector<string> jiffies{};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "cpu") {
        while(linestream >> value) {
          jiffies.push_back(value);
        }
      }
      return jiffies;
    }
  }
  return jiffies;
}

int LinuxParser::TotalProcesses() { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return stoi(value);
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value; 
      if (key == "procs_running") {
        return stoi(value);
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + "/" + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    return line;
  }
  return line;
}

string LinuxParser::Ram(int pid) {
  string line, key, value{""};
  std::ifstream filestream(kProcDirectory + to_string(pid) + "/" + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") {
        return value;
      }
    }
  }
  return value;
}

string LinuxParser::Uid(int pid) {
  string line, key, value{""};
  std::ifstream filestream(kProcDirectory + to_string(pid) + "/" + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        return value;
      }
    }
  }
  return value;
}

string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string line, key, value{""};
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      std::getline(linestream, value, ':');
      std::getline(linestream, key, ':');
      std::getline(linestream, key, ':');
      if (key == uid) {
        return value;
      }
    }
  }
  return value;
}

long LinuxParser::UpTime(int pid) {
  string line, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + "/" + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    int count = 0;
    while( (linestream >> value) && ++count <= 21 );
    return stol(value);
  }
  return 0;
}