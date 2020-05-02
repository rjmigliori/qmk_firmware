#include "hidthread.h"
#include <stdio.h>

HidThread::HidThread(Communication &comm, QObject *parent) : QThread(parent), comm(comm)
{
    keep_scanning = false;
    abort = false;
    enter_bootloader_path = "";
    autoenter_mode = false;
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

void HidThread::run()
{
    forever {
        mutex.lock();
        bool l_keep_scanning, l_abort, nothing_to_do, l_autoenter_mode;
        std::string l_enter_bootloader_path;
        do {
            l_keep_scanning = this->keep_scanning;
            l_enter_bootloader_path = this->enter_bootloader_path;
            l_autoenter_mode = this->autoenter_mode;
            l_abort = this->abort;
            nothing_to_do = (!l_keep_scanning) && (!l_abort) && (l_enter_bootloader_path.size()==0) && (!l_autoenter_mode);
            if (nothing_to_do) {
                printf("Sleeping\n");
                condition.wait(&mutex);
            }
        } while (nothing_to_do);
        mutex.unlock();
        if (l_abort)
            return;
        if (l_enter_bootloader_path.size() != 0)
        {
            try {
                Device dev = comm.open(l_enter_bootloader_path);
                dev.enterBootloader();
            } catch (const std::runtime_error &e1) {
                emit reportError(e1.what());
            }
            mutex.lock();
            this->enter_bootloader_path = "";
            mutex.unlock();
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
                        Device dev = comm.open(value);
                        dev.enterBootloader();
                    } catch (...)
                    {
                        // Ignore errors
                    }
                }
            }
        }
    }
}

