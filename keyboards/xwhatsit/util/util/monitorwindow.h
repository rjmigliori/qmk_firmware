#ifndef MONITORWINDOW_H
#define MONITORWINDOW_H

#include <QDialog>
#include <QGraphicsScene>
#include <QJsonDocument>

namespace Ui {
class MonitorWindow;
}

class MonitorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MonitorWindow(QWidget *parent = nullptr);
    ~MonitorWindow();
    void loadLayout(const QString &name);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void on_layoutSel_activated(const QString &arg1);

private:
    Ui::MonitorWindow *ui;
    QGraphicsScene scene;
    QJsonDocument doc;
};

#endif // MONITORWINDOW_H
