#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

using std::vector;

class Processor {
public:
    float Utilization();

private:
    long prev_active_jiffies{0};
    long prev_idle_jiffies{0};
};

#endif