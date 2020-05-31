#include "signal_level.h"
#include "ui_signal_level.h"
#include <QGraphicsView>
#include <QPainter>
#include <QFile>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <iostream>
#include "kbd_defs.h"

SignalLevelMonitorWindow::SignalLevelMonitorWindow(HidThread &thread, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignalLevelMonitorWindow),
    thread(thread)
{
    ui->setupUi(this);
    keyboard = nullptr;
    connect(&thread, &HidThread::keyboardName, this, &SignalLevelMonitorWindow::on_keyboardName);
    connect(&thread, &HidThread::reportMonitorError, this, &SignalLevelMonitorWindow::on_reportMonitorError);
    connect(&thread, &HidThread::reportSignalLevel, this, &SignalLevelMonitorWindow::on_signallevel);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
}

void SignalLevelMonitorWindow::ShowContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);
    QAction action1("Close", this);
    connect(&action1, SIGNAL(triggered()), this, SLOT(close()));
    contextMenu.addAction(&action1);
    contextMenu.exec(mapToGlobal(pos));
}

SignalLevelMonitorWindow::~SignalLevelMonitorWindow()
{
    delete ui;
}

void SignalLevelMonitorWindow::loadLayout(QString name)
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
    this->signal_level = std::vector<std::vector<uint16_t>>(static_cast<unsigned long>(keyboard->rows), std::vector<uint16_t>(static_cast<unsigned long>(keyboard->cols), 0xFFFFU));
    this->max_signal_level = std::vector<std::vector<uint16_t>>(static_cast<unsigned long>(keyboard->rows), std::vector<uint16_t>(static_cast<unsigned long>(keyboard->cols), 0xFFFFU));
    this->min_signal_level = std::vector<std::vector<uint16_t>>(static_cast<unsigned long>(keyboard->rows), std::vector<uint16_t>(static_cast<unsigned long>(keyboard->cols), 0xFFFFU));
}

QColor SignalLevelMonitorWindow::getColor(uint16_t value, uint16_t mins, uint16_t maxs)
{
    double scale = (value - mins + 0.0) / (maxs - mins);
    uint8_t red = static_cast<uint8_t>(0xff * (1 - scale) + 0xc0 * scale);
    uint8_t green = static_cast<uint8_t>(0xff * scale + 0xc0 * (1 - scale));
    uint8_t blue = 0xc0;
    return QColor(red, green, blue);
}

void SignalLevelMonitorWindow::paintEvent(QPaintEvent *event)
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
            uint16_t mins = 0xffffu, maxs = 0xffffu;
            for (j=0;j<layout->n_keys;j++)
            {
                unsigned int col = layout->keys[j].col;
                unsigned int row = layout->keys[j].row;
                if (signal_level[row][col]!=0xffffu)
                {
                    if ((mins == 0xffffu) || (mins > min_signal_level[row][col]))
                    {
                        mins = min_signal_level[row][col];
                    }
                    if ((maxs == 0xffffu) || (maxs < max_signal_level[row][col]))
                    {
                        maxs = max_signal_level[row][col];
                    }

                }
            }
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
                painter.setPen(QPen(Qt::black));
                painter.setBrush(Qt::NoBrush);
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
                if ((signal_level[row][col]!=0xffffu) && (mins!=maxs))
                {
                    painter.setPen(Qt::NoPen);
                    QString maxstring = QString::number(max_signal_level[row][col]);
                    QString currstring = QString::number(signal_level[row][col]);
                    QString minstring = QString::number(min_signal_level[row][col]);
                    painter.setBrush(QBrush(getColor(max_signal_level[row][col], mins, maxs)));
                    QRectF recti_max(x+1, y+1, w-1, (h-1) / 3);
                    painter.drawRect(recti_max);
                    painter.setBrush(QBrush(getColor(signal_level[row][col], mins, maxs)));
                    QRectF recti_curr(x+1, y+1 + (h-1) / 3, w-1, (h-1) / 3);
                    painter.drawRect(recti_curr);
                    painter.setBrush(QBrush(getColor(min_signal_level[row][col], mins, maxs)));
                    QRectF recti_min(x+1, y+1 + (h-1) / 3 *2, w-1, h-1 - ((h-1) / 3) * 2);
                    painter.drawRect(recti_min);
                    painter.setPen(Qt::black);
                    painter.drawText(recti_max, Qt::AlignCenter, maxstring);
                    painter.drawText(recti_curr, Qt::AlignCenter, currstring);
                    painter.drawText(recti_min, Qt::AlignCenter, minstring);

                } else {
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(QBrush(QColor("#FFFFFF")));
                    QRectF recti(x+1, y+1, w-1, h-1);
                    painter.drawRect(recti);
                    if (signal_level[row][col]!=0xffffu)
                    {
                        painter.setPen(Qt::black);
                        QString currstring = QString::number(signal_level[row][col]);
                        painter.drawText(recti, Qt::AlignCenter, currstring);
                    }
                }
            }
            break;
        }
    }
}

void SignalLevelMonitorWindow::on_layoutSel_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    this->repaint();
}

void SignalLevelMonitorWindow::on_keyboardName(std::string name)
{
    loadLayout(QString::fromStdString(name));
    this->repaint();
}

void SignalLevelMonitorWindow::on_signallevel(std::vector<uint16_t> data)
{
    uint16_t col = data[0];
    uint16_t row = data[1];
    unsigned int i;
    bool different = false;
    for (i=2;i<data.size();i++)
    {
        if (signal_level[row][col] != data[i])
        {
            different = true;
        }
        signal_level[row][col] = data[i];
        if ((min_signal_level[row][col]==0xffffu) || (data[i] < min_signal_level[row][col]))
        {
            min_signal_level[row][col] = data[i];
        }
        if ((max_signal_level[row][col]==0xffffu) || (data[i] > max_signal_level[row][col]))
        {
            max_signal_level[row][col] = data[i];
        }
        col += 1;
        if (col >= keyboard->cols)
        {
            col -= keyboard->cols;
            row += 1;
        }
        if (row >= keyboard->rows)
        {
            break;
        }
    }
    if (different)
        this->repaint();
}

void SignalLevelMonitorWindow::on_reportMonitorError(std::string error_message)
{
    QMessageBox::critical(this, "Error", error_message.c_str());
}

void SignalLevelMonitorWindow::on_SignalLevelMonitorWindow_finished(int result)
{
    Q_UNUSED(result);
    thread.closeMonitoredDevice();
}