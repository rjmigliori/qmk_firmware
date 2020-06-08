#ifndef COLUMNTESTER_H
#define COLUMNTESTER_H

#include <QDialog>
#include <QPoint>
#include <QPushButton>
#include <vector>
#include "hidthread.h"

namespace Ui {
class ColumnTester;
}

class ColumnTester : public QDialog
{
    Q_OBJECT

public:
    explicit ColumnTester(HidThread &thread, std::string path, QWidget *parent = nullptr);
    ~ColumnTester();

private slots:
    void onButtonClicked();
    void ShowContextMenu(const QPoint &pos);

private:
    Ui::ColumnTester *ui;
    std::vector<QPushButton *> buttons;
    HidThread &thread;
    std::string path;
};

#endif // COLUMNTESTER_H
