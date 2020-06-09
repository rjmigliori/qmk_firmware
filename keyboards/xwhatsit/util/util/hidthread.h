#ifndef HIDTHREAD_H
#define HIDTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <vector>
#include <string>
#include "communication.h"
#include "device.h"

class HidThread : public QThread
{
    Q_OBJECT

public:
    HidThread(Communication &comm, QObject *parent = nullptr);
    ~HidThread() override;
    bool setScanning(bool enabled);
    bool setAutoEnter(bool enabled);
    void enterBootloader(std::string path);
    void monitor(std::string path);
    void signalLevel(std::string path);
    void eraseEeprom(std::string path);
    void closeMonitoredDevice();
    void shiftData(std::string path, uint32_t shdata);
    void enableKeyboard(std::string path);
    Device *connectToDevice(std::string path);
signals:
    void scannedDevices(std::vector<std::string> devices);
    void reportError(std::string error_message);
    void reportInfo(std::string error_message);
    void keyboardName(std::string name);
    void thresholds(std::vector<std::vector<uint8_t>>);
    void keystate(std::vector<uint8_t>);
    void reportMonitorError(std::string error_message);
    void reportSignalLevel(std::vector<uint16_t>);

protected:
    void run() override;

private:
    bool abort;
    bool keep_scanning;
    bool autoenter_mode;
    bool close_monitored_device;
    uint32_t shift_data;
    std::string shift_data_path;
    std::string enter_bootloader_path;
    std::string monitor_path;
    std::string signal_level_path;
    std::string erase_eeprom_path;
    std::string enable_keyboard_path;
    QMutex mutex;
    QWaitCondition condition;
    Communication &comm;
};

#endif // HIDTHREAD_H
