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
    void disableKeyboard();
    void enableKeyboard();
    uint32_t getVersion();
    bool isVersionAtLeast(uint8_t major, uint8_t mid, uint16_t minor);
    void assertVersionIsAtLeast(uint8_t major, uint8_t mid, uint16_t minor);
    void eraseEeprom();
    std::vector<std::vector<uint8_t>> getThresholds();
    std::vector<uint8_t> getKeyState();
    std::vector<uint8_t> getKeyboardDetails();
    std::vector<uint16_t> getSignalValue(uint8_t col, uint8_t row);
    std::string getKeyboardFilename();
private:
    hid_device *device = NULL;
    QMutex &mutex;
    bool xwhatsit_original_firmware;
    uint32_t version;
};

#endif // DEVICE_H
