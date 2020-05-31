#include "monitorwindow.h"
#include "ui_monitorwindow.h"
#include <QGraphicsView>
#include <QPainter>
#include <QFile>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <iostream>
#include "kbd_defs.h"

MonitorWindow::MonitorWindow(HidThread &thread, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MonitorWindow),
    thread(thread)
{
    ui->setupUi(this);
    keyboard = nullptr;
    connect(&thread, &HidThread::keyboardName, this, &MonitorWindow::on_keyboardName);
    connect(&thread, &HidThread::reportMonitorError, this, &MonitorWindow::on_reportMonitorError);
    connect(&thread, &HidThread::thresholds, this, &MonitorWindow::on_thresholds);
    connect(&thread, &HidThread::keystate, this, &MonitorWindow::on_keystate);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
}

void MonitorWindow::ShowContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);
    QAction action1("Close", this);
    connect(&action1, SIGNAL(triggered()), this, SLOT(close()));
    contextMenu.addAction(&action1);
    contextMenu.exec(mapToGlobal(pos));
}

MonitorWindow::~MonitorWindow()
{
    delete ui;
}

void MonitorWindow::loadLayout(QString name)
{
    int i;
    if (name.compare(QString("keyboards/xwhatsit/brand_new_model_f_keyboards/f62/f62.c")) == 0)
    {
        name = QString("keyboards/xwhatsit/brand_new_model_f/f62/wcass/wcass.c");
    }
    if (name.compare(QString("keyboards/xwhatsit/brand_new_model_f_keyboards/f77/f77.c")) == 0)
    {
        name = QString("keyboards/xwhatsit/brand_new_model_f/f77/wcass/wcass.c");
    }
    ui->layoutSel->clear();
    for (i=0;i<n_keyboards;i++)
    {
        if (name.compare(QString(keyboards[i].kbd_name))==0)
        {
            keyboard = &keyboards[i];
            int j;
            for (j=0;j<keyboard->n_layouts;j++)
            {
                ui->layoutSel->addItem(QString(keyboard->layouts[j].lay_name));
            }
            break;
        }
    }
    QString name_simp = name;
    if (name.startsWith("keyboards/")) name_simp = name.mid(strlen("keyboards/"));
    int lastslash = name_simp.lastIndexOf("/");
    if (lastslash > 0) name_simp = name_simp.left(lastslash);
    ui->label_keyboardname->setText(QString("Keyboard: ") + name_simp);
    ui->layoutSel->setEnabled(true);
    if (!keyboard) on_reportMonitorError("Unknown keyboard (you may need to update the util version)!");
    keyboard_width_uis = 0;
    keyboard_height_uis = 0;
    for (i=0;i<keyboard->n_layouts;i++)
    {
        const struct lay_def *layout = &keyboard->layouts[i];
        int j;
        for (j=0;j<layout->n_keys;j++)
        {
            int w = static_cast<int>(layout->keys[j].x + layout->keys[j].w + 0.5);
            int h = static_cast<int>(layout->keys[j].y + layout->keys[j].h + 0.5);
            if (w > keyboard_width_uis) keyboard_width_uis = w;
            if (h > keyboard_height_uis) keyboard_height_uis = h;
        }
    }
    this->setMinimumSize(static_cast<int>(keyboard_width_uis * MIN_HORIZONTAL_SCALE + 2 * HORIZONTAL_MARGIN),
                         ui->last_label->geometry().y() + ui->last_label->geometry().height() +
                             static_cast<int>(keyboard_height_uis * MIN_VERTICAL_SCALE + 2 * VERTICAL_MARGIN));
    is_was_key_pressed = std::vector<std::vector<int>>(static_cast<unsigned long>(keyboard->rows), std::vector<int>(static_cast<unsigned long>(keyboard->cols), 0));
}

void MonitorWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (!keyboard) return;
    QPainter painter(this);
    int xadd = HORIZONTAL_MARGIN;
    int yadd = ui->last_label->geometry().y() + ui->last_label->geometry().height() + VERTICAL_MARGIN;
    int maxy = 0;
    int maxx = 0;
    double scale_x = (1. * this->width() - 2 * HORIZONTAL_MARGIN) / (keyboard_width_uis);
    double scale_y = (1. * this->height() - VERTICAL_MARGIN - yadd) / keyboard_height_uis;
    int i;
    for (i=0;i<keyboard->n_layouts;i++)
    {
        if (ui->layoutSel->currentText().compare(QString(keyboard->layouts[i].lay_name))==0)
        {
            const struct lay_def *layout = &keyboard->layouts[i];
            int j;
            for (j=0;j<layout->n_keys;j++)
            {
                unsigned int col = layout->keys[j].col;
                unsigned int row = layout->keys[j].row;
                int x_ = static_cast<int>(layout->keys[j].x * 8 + 0.5);
                int y_ = static_cast<int>(layout->keys[j].y * 8 + 0.5);
                int w_ = static_cast<int>(layout->keys[j].w * 8 + 0.5);
                int h_ = static_cast<int>(layout->keys[j].h * 8 + 0.5);
                int x = static_cast<int>(scale_x * x_ / 8 + 0.5);
                int y = static_cast<int>(scale_y * y_ / 8 + 0.5);
                int w = static_cast<int>(scale_x * (x_ + w_) / 8 + 0.5) - x;
                int h = static_cast<int>(scale_y * (y_ + h_) / 8 + 0.5) - y;
                y += yadd;
                x += xadd;
                QPen textColor(Qt::black);
                painter.setPen(QPen(Qt::black));
                switch (is_was_key_pressed[row][col])
                {
                    case 1:
                    case 3:
                        textColor = QPen(Qt::white);
                        painter.setBrush(QBrush(QColor("#008000")));
                        break;
                    case 2:
                        painter.setBrush(QBrush(QColor("#00FF00")));
                        break;
                    default:
                        painter.setBrush(QBrush(QColor("#FFFFFF")));
                        break;
                }

                QRectF rect(x, y, w, h);
                painter.drawRect(rect);
                if (y+h > maxy)
                {
                    maxy = y + h;
                }
                if (x+w > maxx)
                {
                    maxx = x + w;
                }
                if (thresholds.size())
                {
                    uint16_t threshold = get_threshold(col, row);
                    painter.setPen(textColor);
                    painter.drawText(rect, Qt::AlignCenter, QString::number(threshold));
                }
            }
            break;
        }
    }
}

void MonitorWindow::on_layoutSel_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    this->repaint();
}

void MonitorWindow::on_keyboardName(std::string name)
{
    loadLayout(QString::fromStdString(name));
    this->repaint();
}

void MonitorWindow::on_thresholds(std::vector<std::vector<uint8_t>> thresholds)
{
    this->thresholds = thresholds;
    this->repaint();
}

void MonitorWindow::on_keystate(std::vector<uint8_t> data)
{
    unsigned int col, row;
    int different = 0;
    if (!keyboard) return;
    unsigned int bytes_per_row = 1;
    if (keyboard->cols > 16)
    {
        bytes_per_row = 4;
    } else if (keyboard->cols > 8)
    {
        bytes_per_row = 2;
    }
    for (col=0; col < keyboard->cols; col++)
    {
        for (row=0; row < keyboard->rows; row++)
        {
            uint8_t pressed = (data[col / 8 + row * bytes_per_row] >> (col % 8)) & 1;
            int new_is_was_key_pressed = is_was_key_pressed[row][col];
            if (pressed) new_is_was_key_pressed |= 3;
            else new_is_was_key_pressed &= ~1;
            if (is_was_key_pressed[row][col] != new_is_was_key_pressed) different = 1;
            is_was_key_pressed[row][col]= new_is_was_key_pressed;
        }
    }
    if (different) this->repaint();
}

uint16_t MonitorWindow::get_threshold(unsigned int col, unsigned int row)
{
    if (thresholds[0][0] == 0) return static_cast<uint16_t>((thresholds[0][1] | (thresholds[0][2] << 8)));
    unsigned int i;
    unsigned int bytes_per_row = 1;
    if (keyboard->cols > 16)
    {
        bytes_per_row = 4;
    } else if (keyboard->cols > 8)
    {
        bytes_per_row = 2;
    }
    for (i=0;i<thresholds[0][0]; i++)
    {
        if ((thresholds[i][3 + col / 8 + row * bytes_per_row] >> (col % 8)) & 1)
        {
            return static_cast<uint16_t>(thresholds[i][1] | (thresholds[i][2] << 8));
        }
    }
    return 0;
}

void MonitorWindow::on_reportMonitorError(std::string error_message)
{
    QMessageBox::critical(this, "Error", error_message.c_str());
}

void MonitorWindow::on_MonitorWindow_finished(int result)
{
    Q_UNUSED(result);
    thread.closeMonitoredDevice();
}
