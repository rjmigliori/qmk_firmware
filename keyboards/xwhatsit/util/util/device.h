#ifndef DEVICE_H
#define DEVICE_H

#include <hidapi.h>
#include <string>
#include <vector>
#include <QMutex>

extern const std::string XWHATSIT_ENDING_STRING;
bool is_xwhatsit_original_firmware_path(std::string path);

class Device
{
public:
    Device(std::string path, QMutex &mutex);
    virtual ~Device();
    void close();
    void enterBootloader();
private:
    hid_device *device = NULL;
    QMutex &mutex;
    bool xwhatsit_original_firmware;
};

#endif // DEVICE_H
