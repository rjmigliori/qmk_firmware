#ifndef HIDTHREAD_H
#define HIDTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <vector>
#include <string>
#include "communication.h"

class HidThread : public QThread
{
    Q_OBJECT

public:
    HidThread(Communication &comm, QObject *parent = nullptr);
    ~HidThread() override;
    bool setScanning(bool enabled);
    bool setAutoEnter(bool enabled);
    void enterBootloader(std::string path);
signals:
    void scannedDevices(std::vector<std::string> devices);
    void reportError(std::string error_message);

protected:
    void run() override;

private:
    bool abort;
    bool keep_scanning;
    bool autoenter_mode;
    std::string enter_bootloader_path;
    QMutex mutex;
    QWaitCondition condition;
    Communication &comm;
};

#endif // HIDTHREAD_H
