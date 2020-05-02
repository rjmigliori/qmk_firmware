#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <hidapi.h>
#include <string>
#include <vector>
#include "device.h"

class Communication
{
public:
    Communication();
    virtual ~Communication();
    std::vector<std::string> scan();
    Device open(std::string path);
private:
    QMutex mutex;
};

#endif // COMMUNICATION_H
