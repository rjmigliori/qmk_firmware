#include "hidthread.h"
#include <stdio.h>
#include <iostream>

HidThread::HidThread(Communication &comm, QObject *parent) : QThread(parent), comm(comm)
{
    keep_scanning = false;
    abort = false;
    enter_bootloader_path = "";
    autoenter_mode = false;
    close_monitored_device = false;
}

HidThread::~HidThread()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}

bool HidThread::setScanning(bool enabled)
{
    QMutexLocker locker(&mutex);
    bool previous = this->keep_scanning;
    this->keep_scanning = enabled;
    condition.wakeOne();
    return previous;
}

bool HidThread::setAutoEnter(bool enabled)
{
    QMutexLocker locker(&mutex);
    bool previous = autoenter_mode;
    autoenter_mode = enabled;
    return previous;
}

void HidThread::enterBootloader(std::string path)
{
     QMutexLocker locker(&mutex);
     this->enter_bootloader_path = path;
     condition.wakeOne();
}

void HidThread::monitor(std::string path)
{
    QMutexLocker locker(&mutex);
    this->monitor_path = path;
    condition.wakeOne();
}

void HidThread::signalLevel(std::string path)
{
    QMutexLocker locker(&mutex);
    this->signal_level_path = path;
    condition.wakeOne();
}

void HidThread::eraseEeprom(std::string path)
{
    QMutexLocker locker(&mutex);
    this->erase_eeprom_path = path;
    condition.wakeOne();
}

void HidThread::closeMonitoredDevice()
{
    QMutexLocker locker(&mutex);
    this->close_monitored_device = true;
    condition.wakeOne();
}

void HidThread::run()
{
    Device *monitoredDevice = nullptr;
    Device *signalLevelDevice = nullptr;
    uint8_t cols=0, rows=0, current_col=0, current_row=0;
    forever {
        mutex.lock();
        bool l_keep_scanning, l_abort, nothing_to_do, l_autoenter_mode, l_close_monitored_device;
        std::string l_enter_bootloader_path, l_monitor_path, l_erase_eeprom_path, l_signal_level_path;
        do {
            l_keep_scanning = this->keep_scanning;
            l_enter_bootloader_path = this->enter_bootloader_path;
            l_monitor_path = this->monitor_path;
            l_signal_level_path = this->signal_level_path;
            l_autoenter_mode = this->autoenter_mode;
            l_close_monitored_device = this->close_monitored_device;
            l_abort = this->abort;
            l_erase_eeprom_path = this->erase_eeprom_path;
            nothing_to_do = (!l_keep_scanning) &&
                            (!l_abort) &&
                            (l_enter_bootloader_path.size()==0) &&
                            (l_monitor_path.size()==0) &&
                            (!l_autoenter_mode) &&
                            (monitoredDevice == nullptr) &&
                            (!l_close_monitored_device) &&
                            (l_erase_eeprom_path.size() == 0) &&
                            (l_signal_level_path.size() == 0) &&
                            (signalLevelDevice == nullptr);
            if (nothing_to_do) {
                condition.wait(&mutex);
            }
        } while (nothing_to_do);
        mutex.unlock();
        if (l_abort)
            return;
        if (l_enter_bootloader_path.size() != 0)
        {
            try {
                QScopedPointer<Device> dev(comm.open(l_enter_bootloader_path));
                dev.data()->enterBootloader();
            } catch (const std::runtime_error &e1) {
                emit reportError(e1.what());
            }
            mutex.lock();
            this->enter_bootloader_path = "";
            mutex.unlock();
        }
        if (l_erase_eeprom_path.size() != 0)
        {
            try {
                QScopedPointer<Device> dev(comm.open(l_erase_eeprom_path));
                dev.data()->assertVersionIsAtLeast(2, 0, 1);
                dev.data()->eraseEeprom();
                emit reportInfo("EEPROM Erase DONE!");
            } catch (const std::runtime_error &e1) {
                emit reportError(e1.what());
            }
            mutex.lock();
            this->erase_eeprom_path = "";
            mutex.unlock();
        }
        if (l_monitor_path.size() != 0)
        {
            try {
                QScopedPointer<Device> dev(comm.open(l_monitor_path));
                dev.data()->assertVersionIsAtLeast(2, 0, 0);
                std::string name = dev.data()->getKeyboardFilename();
                emit keyboardName(name);
                emit thresholds(dev.data()->getThresholds());
                dev.data()->disableKeyboard();
                monitoredDevice = dev.take();
            } catch (const std::runtime_error &e1) {
                emit reportMonitorError(e1.what());
            }
            mutex.lock();
            this->monitor_path = "";
            mutex.unlock();
        }
        if (l_signal_level_path.size() != 0)
        {
            try {
                QScopedPointer<Device> dev(comm.open(l_signal_level_path));
                dev.data()->assertVersionIsAtLeast(2, 0, 2);
                std::string name = dev.data()->getKeyboardFilename();
                emit keyboardName(name);
                dev.data()->disableKeyboard();
                std::vector<uint8_t> details = dev.data()->getKeyboardDetails();
                cols = details[0];
                rows = details[1];
                current_col = current_row = 0;
                signalLevelDevice = dev.take();
            } catch (const std::runtime_error &e1) {
                emit reportMonitorError(e1.what());
            }
            mutex.lock();
            this->signal_level_path = "";
            mutex.unlock();
        }
        if (l_close_monitored_device)
        {
            if (monitoredDevice != nullptr)
            {
                monitoredDevice->enableKeyboard();
                delete monitoredDevice;
                monitoredDevice = nullptr;
            }
            if (signalLevelDevice != nullptr)
            {
                signalLevelDevice->enableKeyboard();
                delete signalLevelDevice;
                signalLevelDevice = nullptr;
            }
            mutex.lock();
            this->close_monitored_device = false;
            mutex.unlock();
        }
        if (monitoredDevice != nullptr)
        {
            emit keystate(monitoredDevice->getKeyState());
        }
        if (signalLevelDevice != nullptr)
        {
            std::vector<uint16_t> signal_levels = signalLevelDevice->getSignalValue(current_col, current_row);
            unsigned long cnt = signal_levels.size();
            signal_levels.insert(signal_levels.begin(), current_row);
            signal_levels.insert(signal_levels.begin(), current_col);
            current_col += cnt;
            while (current_col >= cols) {
                current_col -= cols;
                current_row += 1;
            }
            if (current_row >= rows)
            {
                current_row = 0;
                current_col = 0;
            }
            emit reportSignalLevel(signal_levels);
        }
        if (l_keep_scanning)
        {
            msleep(10);
            std::vector<std::string> devices = comm.scan();
            emit scannedDevices(devices);
            if (l_autoenter_mode)
            {
                for(auto value : devices)
                {
                    try {
                        QScopedPointer<Device> dev(comm.open(value));
                        dev.data()->enterBootloader();
                    } catch (...)
                    {
                        // Ignore errors
                    }
                }
            }
        }
    }
}

