#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <QMessageBox>
#include <QMetaType>

Q_DECLARE_METATYPE(std::vector<std::string>)
Q_DECLARE_METATYPE(std::string)

MainWindow::MainWindow(Communication &comm, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    comm(comm),
    thread(comm)
{
    ui->setupUi(this);
    qRegisterMetaType<std::vector<std::string>>("vector_of_strings");
    connect(&thread, &HidThread::scannedDevices, this, &MainWindow::on_updateScannedDevices);
    qRegisterMetaType<std::string>("std_string");
    connect(&thread, &HidThread::reportError, this, &MainWindow::on_reportError);
    thread.start();
    thread.setScanning(true);
}

void MainWindow::on_updateScannedDevices(std::vector<std::string> devices)
{
    int i;
    for (i=ui->listWidget->count()-1;i>=0;i--)
    {
        bool found = false;
        for (auto value : devices)
        {
            if (ui->listWidget->item(i)->text().toStdString().compare(value)==0)
            {
                found = true;
            }
        }
        if (!found) {
            delete ui->listWidget->takeItem(i);
        }
    }
    int countBefore = ui->listWidget->count();
    for (auto value : devices)
    {
        bool found = false;
        for (i=ui->listWidget->count()-1;i>=0;i--)
        {
            if (ui->listWidget->item(i)->text().toStdString().compare(value)==0)
            {
                found = true;
            }
        }
        if (!found)
        {
            ui->listWidget->addItem(QString::fromStdString(value));
        }
    }
    if ((countBefore == 0) && (ui->listWidget->count() == 1))
    {
        ui->listWidget->setCurrentRow(0);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_listWidget_itemSelectionChanged()
{
    bool is_xwhatsit = false;
    QListWidgetItem *item = ui->listWidget->currentItem();
    if (item)
    {
        is_xwhatsit = is_xwhatsit_original_firmware_path(item->text().toStdString());
    }
    bool enabled = !!ui->listWidget->currentItem();
    ui->enterBootloaderPushbutton->setEnabled(enabled);
    //ui->keypressMinotorPushButton->setEnabled(enabled && !is_xwhatsit);
    Q_UNUSED(is_xwhatsit);
}

void MainWindow::on_enterBootloaderPushbutton_clicked()
{
    thread.enterBootloader(ui->listWidget->currentItem()->text().toStdString());
}

void MainWindow::on_reportError(std::string error_message)
{
    QMessageBox::critical(this, "Error", error_message.c_str());
}

void MainWindow::on_keypressMinotorPushButton_clicked()
{
    mw.loadLayout("../../brand_new_model_f_keyboards/f62/info.json");
    bool previousScanning = thread.setScanning(false);
    ui->listWidget->setEnabled(false);
    mw.exec();
    ui->listWidget->setEnabled(true);
    thread.setScanning(previousScanning);
}

void MainWindow::on_autoEnterModeCheckBox_stateChanged(int arg1)
{
    thread.setAutoEnter(arg1);
}
