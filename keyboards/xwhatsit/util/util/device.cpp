#include "device.h"
#include <stdexcept>
#include <string.h>
#include "../../util_comm.h"

const std::string XWHATSIT_ENDING_STRING = " (Running original xwhatsit firmware)";

static const uint8_t magic[] = UTIL_COMM_MAGIC;

bool is_xwhatsit_original_firmware_path(std::string path)
{
    try {
        if (0==path.compare(path.length()-XWHATSIT_ENDING_STRING.length(), XWHATSIT_ENDING_STRING.length(), XWHATSIT_ENDING_STRING))
            return true;
    } catch (std::out_of_range &e)
    {
        // Ignore error
    }
    return false;
}

Device::Device(std::string path, QMutex &mutex) :
    mutex(mutex)
{
    QMutexLocker locker(&mutex);
    std::string real_path = path;
    xwhatsit_original_firmware = is_xwhatsit_original_firmware_path(path);
    if (xwhatsit_original_firmware)
    {
        real_path = path.substr(0, path.length() - XWHATSIT_ENDING_STRING.length());
    }

    device = hid_open_path(real_path.c_str());
    if (!device)
    {
        throw std::runtime_error("Error opening device");
    }
    if (!xwhatsit_original_firmware)
    {
        uint8_t data[33];
        data[0] = 0;
        memcpy(data + 1, magic, sizeof(magic));
        data[2+1] = UTIL_COMM_GET_VERSION;
        if (-1==hid_write(device, data, sizeof(data)))
        {
            hid_close(device);
            throw std::runtime_error("hid_write failed to get version");
        }
        if ((sizeof(data)-1)!=hid_read_timeout(device, data, sizeof(data)-1, 1000))
        {
            hid_close(device);
            throw std::runtime_error("hid_read failed to get version");
        }
        if ((data[0] != magic[0]) || (data[1] != magic[1]))
        {
            hid_close(device);
            throw std::runtime_error("hid_read did not receive back magic");
        }
        if (data[2] != UTIL_COMM_RESPONSE_OK)
        {
            hid_close(device);
            throw std::runtime_error("hid_read did not return OK response");
        }
        if (data[3] != UTIL_COMM_VERSION)
        {
            hid_close(device);
            throw std::runtime_error("Protocol version is wrong");
        }
    }
}

Device::~Device()
{
    close();
}

void Device::close()
{
    QMutexLocker locker(&mutex);
    if (device)
    {
        hid_close(device);
        device = NULL;
    }
}

void Device::enterBootloader()
{
    {
        QMutexLocker locker(&mutex);
        if (!device) std::runtime_error("device already closed");
        if (xwhatsit_original_firmware)
        {
            uint8_t data[9] = {0, 2, 1, 0, 0, 0, 0, 0, 0};
            hid_write(device, data, sizeof(data));
        } else
        {
            uint8_t data[33];
            data[0] = 0;
            memcpy(data + 1, magic, sizeof(magic));
            data[2+1] = UTIL_COMM_ENTER_BOOTLOADER;
            if (-1==hid_write(device, data, sizeof(data)))
            {
                printf("hid error: %ls\n", hid_error(device));
                hid_close(device);
                device = NULL;
                throw std::runtime_error("hid_write failed to enter bootloader");
            }
        }
    }
    close();
}
