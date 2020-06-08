#ifndef MONITORWINDOW_H
#define MONITORWINDOW_H

#include <QDialog>
#include <QGraphicsScene>
#include <QJsonDocument>
#include "hidthread.h"

namespace Ui {
class MonitorWindow;
}

class MonitorWindow : public QDialog
{
    Q_OBJECT

public:
    explicit MonitorWindow(HidThread &thread, QWidget *parent = nullptr);
    ~MonitorWindow();
    void loadLayout(QString name);

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void on_layoutSel_activated(const QString &arg1);
    void on_keyboardName(std::string name);
    void on_thresholds(std::vector<std::vector<uint8_t>>);
    void on_keystate(std::vector<uint8_t>);
    void on_reportMonitorError(std::string error_message);

    void on_MonitorWindow_finished(int result);
    void ShowContextMenu(const QPoint &pos);

private:
    Ui::MonitorWindow *ui;
    QGraphicsScene scene;
    QJsonDocument doc;
    const struct kbd_def *keyboard;
    const struct lay_def *current_layout;
    HidThread &thread;
    std::vector<std::vector<uint8_t>> thresholds;
    uint16_t get_threshold(unsigned int col, unsigned int row);
    void updateCurrentLayout();
    void displaySquare(int x, int y, int w, int h, unsigned int col, unsigned int row, QPainter &painter);
    void setMinimumSizeUnits(unsigned int width_units, unsigned int height_units);
    std::vector<std::vector<int>> is_was_key_pressed;

    unsigned int keyboard_width_uis;
    unsigned int keyboard_height_uis;
    static constexpr double MIN_HORIZONTAL_SCALE = 32;
    static constexpr double MIN_VERTICAL_SCALE = 32;
    static const int HORIZONTAL_MARGIN = 10;
    static const int VERTICAL_MARGIN = 10;
};

#endif // MONITORWINDOW_H
