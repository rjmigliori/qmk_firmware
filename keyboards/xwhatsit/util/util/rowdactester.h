#ifndef ROWDACTESTER_H
#define ROWDACTESTER_H

#include <QDialog>
#include <QMenu>
#include "hidthread.h"

namespace Ui {
class RowDacTester;
}

class RowDacTester : public QDialog
{
    Q_OBJECT

public:
    explicit RowDacTester(HidThread &thread, std::string path, QWidget *parent);
    ~RowDacTester();

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void ShowContextMenu(const QPoint &pos);

    void on_horizontalSlider_valueChanged(int value);

    void on_spinBox_valueChanged(int value);

    void on_horizontalSliderSupply_valueChanged(int value);

    void on_doubleSpinBoxSupply_valueChanged(double value);

    void on_RowDacTester_finished(int result);

    void on_rowstate(uint8_t);

    void on_reportMonitorError(std::string error_message);

    void on_reportRowsAndMaxDac(uint8_t rows, uint16_t max_dac);

private:
    Ui::RowDacTester *ui;
    void update_value(int value);
    void update_powersupply_value(int value_10mV);
    static const int HORIZONTAL_MARGIN = 10;
    static const int VERTICAL_MARGIN = 10;
    HidThread &thread;
    std::string path;
    uint8_t rowstate;
    uint8_t rows;
    uint16_t max_dac;
};

#endif // ROWDACTESTER_H
