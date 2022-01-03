#include <string>

#include "format.h"

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int minutes = seconds/60;
    int hours = minutes/60;
    minutes = minutes - (hours*60);
    seconds = seconds - (hours*3600) - (minutes*60);
    string time = "";
    time += (hours <= 9) ? "0" + to_string(hours) : to_string(hours);
    time += ":";
    time += (minutes <= 9) ? "0" + to_string(minutes) : to_string(minutes);
    time += ":";
    time += (seconds <= 9) ? "0" + to_string(seconds) : to_string(seconds);
    return time;
}