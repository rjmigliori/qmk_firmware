#include "device.h"
#include <stdexcept>
#include <string.h>
#include <stdio.h>
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
        version = static_cast<uint32_t>(data[3]);
        if (version < 1)
        {
            hid_close(device);
            throw std::runtime_error("Protocol version is wrong");
        }
        if (version >= 2)
        {
            version = (static_cast<uint32_t>(data[3]) << 24) |
                      (static_cast<uint32_t>(data[4]) << 16) |
                      (static_cast<uint32_t>(data[5]) << 8 ) |
                      (static_cast<uint32_t>(data[6]) << 0 );
                                                        ;
        } else {
            version = static_cast<uint32_t>(data[3]) << 24;
        }
    } else {
        version = 0;
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

void Device::enableKeyboard()
{
    QMutexLocker locker(&mutex);
    if (xwhatsit_original_firmware)
    {
        throw std::runtime_error("This doesn't work with xwhatsit original firmware");
    }
    uint8_t data[33];
    data[0] = 0;
    memcpy(data + 1, magic, sizeof(magic));
    data[2+1] = UTIL_COMM_ENABLE_KEYBOARD;
    if (-1==hid_write(device, data, sizeof(data)))
    {
        printf("hid error: %ls\n", hid_error(device));
        throw std::runtime_error("hid_write failed to enable keyboard");
    }
    if ((sizeof(data)-1)!=hid_read_timeout(device, data, sizeof(data)-1, 1000))
    {
        printf("hid error: %ls\n", hid_error(device));
        throw std::runtime_error("hid_read failed while enabling keyboard");
    }
    if ((data[0] != magic[0]) || (data[1] != magic[1]))
    {
        throw std::runtime_error("hid_read failed while enabling keyboard -- no magic returned");
    }
    if (data[2] != UTIL_COMM_RESPONSE_OK)
    {
        throw std::runtime_error("hid_read failed while enabling keyboard -- response not okay");
    }
}

void Device::eraseEeprom()
{
    QMutexLocker locker(&mutex);
    if (xwhatsit_original_firmware)
    {
        throw std::runtime_error("This doesn't work with xwhatsit original firmware");
    }
    uint8_t data[33];
    data[0] = 0;
    memcpy(data + 1, magic, sizeof(magic));
    data[2+1] = UTIL_COMM_ERASE_EEPROM;
    if (-1==hid_write(device, data, sizeof(data)))
    {
        printf("hid error: %ls\n", hid_error(device));
        throw std::runtime_error("hid_write failed to erase eeprom");
    }
    if ((sizeof(data)-1)!=hid_read_timeout(device, data, sizeof(data)-1, 1000))
    {
        printf("hid error: %ls\n", hid_error(device));
        throw std::runtime_error("hid_read failed while erasing eeprom");
    }
    if ((data[0] != magic[0]) || (data[1] != magic[1]))
    {
        throw std::runtime_error("hid_read failed while erasing eeprom -- no magic returned");
    }
    if (data[2] != UTIL_COMM_RESPONSE_OK)
    {
        throw std::runtime_error("hid_read failed while erasing eeprom -- response not okay");
    }
}

void Device::disableKeyboard()
{
    QMutexLocker locker(&mutex);
    if (xwhatsit_original_firmware)
    {
        throw std::runtime_error("This doesn't work with xwhatsit original firmware");
    }
    uint8_t data[33];
    data[0] = 0;
    memcpy(data + 1, magic, sizeof(magic));
    data[2+1] = UTIL_COMM_DISABLE_KEYBOARD;
    if (-1==hid_write(device, data, sizeof(data)))
    {
        printf("hid error: %ls\n", hid_error(device));
        throw std::runtime_error("hid_write failed to disable keyboard");
    }
    if ((sizeof(data)-1)!=hid_read_timeout(device, data, sizeof(data)-1, 1000))
    {
        printf("hid error: %ls\n", hid_error(device));
        throw std::runtime_error("hid_read failed while disabling keyboard");
    }
    if ((data[0] != magic[0]) || (data[1] != magic[1]))
    {
        throw std::runtime_error("hid_read failed while disabling keyboard -- no magic returned");
    }
    if (data[2] != UTIL_COMM_RESPONSE_OK)
    {
        throw std::runtime_error("hid_read failed while disabling keyboard -- response not okay");
    }
}

std::vector<std::vector<uint8_t>> Device::getThresholds()
{
    QMutexLocker locker(&mutex);
    std::vector<std::vector<uint8_t>> ret;
    if (xwhatsit_original_firmware)
    {
        throw std::runtime_error("This doesn't work with xwhatsit original firmware");
    }
    uint8_t bins;
    uint8_t current_bin = 0;
    do {
        uint8_t data[33];
        data[0] = 0;
        memcpy(data + 1, magic, sizeof(magic));
        data[2+1] = UTIL_COMM_GET_THRESHOLDS;
        data[3+1] = current_bin;
        if (-1==hid_write(device, data, sizeof(data)))
        {
            printf("hid error: %ls\n", hid_error(device));
            throw std::runtime_error("hid_write failed to disable keyboard");
        }
        if ((sizeof(data)-1)!=hid_read_timeout(device, data, sizeof(data)-1, 1000))
        {
            printf("hid error: %ls\n", hid_error(device));
            throw std::runtime_error("hid_read failed while getting thresholds");
        }
        if ((data[0] != magic[0]) || (data[1] != magic[1]))
        {
            throw std::runtime_error("hid_read failed while getting thresholds -- no magic returned");
        }
        if (data[2] != UTIL_COMM_RESPONSE_OK)
        {
            throw std::runtime_error("hid_read failed while getting thresholds -- response not okay");
        }
        std::vector<uint8_t> trdata(&data[3], &data[32]);
        ret.push_back(trdata);
        bins = data[3];
        current_bin ++;
    } while (current_bin < bins);
    return ret;
}

std::vector<uint8_t> Device::getKeyState()
{
    QMutexLocker locker(&mutex);
    if (xwhatsit_original_firmware)
    {
        throw std::runtime_error("This doesn't work with xwhatsit original firmware");
    }
    uint8_t data[33];
    data[0] = 0;
    memcpy(data + 1, magic, sizeof(magic));
    data[2+1] = UTIL_COMM_GET_KEYSTATE;
    if (-1==hid_write(device, data, sizeof(data)))
    {
        printf("hid error: %ls\n", hid_error(device));
        throw std::runtime_error("hid_write failed to get keystate");
    }
    if ((sizeof(data)-1)!=hid_read_timeout(device, data, sizeof(data)-1, 1000))
    {
        printf("hid error: %ls\n", hid_error(device));
        throw std::runtime_error("hid_read failed while getting keystate");
    }
    if ((data[0] != magic[0]) || (data[1] != magic[1]))
    {
        throw std::runtime_error("hid_read failed while getting keystate -- no magic returned");
    }
    if (data[2] != UTIL_COMM_RESPONSE_OK)
    {
        throw std::runtime_error("hid_read failed while getting keystate -- response not okay");
    }
    std::vector<uint8_t> ret(&data[3], &data[32]);
    return ret;
}


std::vector<uint8_t> Device::getKeyboardDetails()
{
    QMutexLocker locker(&mutex);
    if (xwhatsit_original_firmware)
    {
        throw std::runtime_error("This doesn't work with xwhatsit original firmware");
    }
    uint8_t data[33];
    data[0] = 0;
    memcpy(data + 1, magic, sizeof(magic));
    data[2+1] = UTIL_COMM_GET_KEYBOARD_DETAILS;
    if (-1==hid_write(device, data, sizeof(data)))
    {
        printf("hid error: %ls\n", hid_error(device));
        throw std::runtime_error("hid_write failed to get keyboard details");
    }
    if ((sizeof(data)-1)!=hid_read_timeout(device, data, sizeof(data)-1, 1000))
    {
        printf("hid error: %ls\n", hid_error(device));
        throw std::runtime_error("hid_read failed while getting keyboard details");
    }
    if ((data[0] != magic[0]) || (data[1] != magic[1]))
    {
        throw std::runtime_error("hid_read failed while getting keyboard details -- no magic returned");
    }
    if (data[2] != UTIL_COMM_RESPONSE_OK)
    {
        throw std::runtime_error("hid_read failed while getting keyboard details -- response not okay");
    }
    std::vector<uint8_t> ret(&data[3], &data[32]);
    return ret;
}

std::vector<uint16_t> Device::getSignalValue(uint8_t col, uint8_t row)
{
    QMutexLocker locker(&mutex);
    if (xwhatsit_original_firmware)
    {
        throw std::runtime_error("This doesn't work with xwhatsit original firmware");
    }
    uint8_t data[33];
    data[0] = 0;
    memcpy(data + 1, magic, sizeof(magic));
    data[2+1] = UTIL_COMM_GET_SIGNAL_VALUE;
    data[3+1] = col;
    data[4+1] = row;
    uint8_t count = 14;
    data[5+1] = count;
    if (-1==hid_write(device, data, sizeof(data)))
    {
        printf("hid error: %ls\n", hid_error(device));
        throw std::runtime_error("hid_write failed to get signal value");
    }
    if ((sizeof(data)-1)!=hid_read_timeout(device, data, sizeof(data)-1, 1000))
    {
        printf("hid error: %ls\n", hid_error(device));
        throw std::runtime_error("hid_read failed while getting signal value");
    }
    if ((data[0] != magic[0]) || (data[1] != magic[1]))
    {
        throw std::runtime_error("hid_read failed while getting signal value -- no magic returned");
    }
    if (data[2] != UTIL_COMM_RESPONSE_OK)
    {
        throw std::runtime_error("hid_read failed while getting signal value -- response not okay");
    }
    uint8_t i;
    std::vector<uint16_t> ret;
    for (i=0;i<count;i++)
    {
        uint16_t value = data[3+i*2] | static_cast<uint16_t>(data[3+i*2+1] << 8);
        ret.push_back(value);
    }
    return ret;
}

uint32_t Device::getVersion()
{
    return version;
}

bool Device::isVersionAtLeast(uint8_t major, uint8_t mid, uint16_t minor)
{
    if ((version >> 24) > major) return true;
    if ((version >> 24) < major) return false;
    if (((version >> 16) & 0xff) > mid) return true;
    if (((version >> 16) & 0xff) < mid) return false;
    if ((version & 0xffff) >= minor) return true;
    return false;
}

void Device::assertVersionIsAtLeast(uint8_t major, uint8_t mid, uint16_t minor)
{
    if (!isVersionAtLeast(major, mid, minor))
        throw std::runtime_error("Version of communication protocl is too old. Please update your firmware.");
}

std::string Device::getKeyboardFilename()
{
    QMutexLocker locker(&mutex);
    if (xwhatsit_original_firmware)
    {
        throw std::runtime_error("This doesn't work with xwhatsit original firmware");
    }
    int start = 0;
    std::string s;
    std::string piece;
    do
    {
        uint8_t data[33];
        data[0] = 0;
        memcpy(data + 1, magic, sizeof(magic));
        data[2+1] = UTIL_COMM_GET_KEYBOARD_FILENAME;
        data[4] = static_cast<uint8_t>(start);
        if (-1==hid_write(device, data, sizeof(data)))
        {
            printf("hid error: %ls\n", hid_error(device));
            throw std::runtime_error("hid_write failed to get keyboard firmware");
        }
        if ((sizeof(data)-1)!=hid_read_timeout(device, data, sizeof(data)-1, 1000))
        {
            printf("hid error: %ls\n", hid_error(device));
            throw std::runtime_error("hid_read failed to get keyboard firmware");
        }
        if ((data[0] != magic[0]) || (data[1] != magic[1]))
        {
            throw std::runtime_error("hid_read failed to get keyboard firmware -- no magic returned");
        }
        if (data[2] != UTIL_COMM_RESPONSE_OK)
        {
            throw std::runtime_error("hid_read failed to get keyboard firmware -- response not okay");
        }
        data[32] = 0;
        piece = std::string(reinterpret_cast<const char *>(data + 3));
        s += piece;
        start += piece.size();
    } while ((piece.length() >= 32 - 3) && (start < 256));
    return s;
}
