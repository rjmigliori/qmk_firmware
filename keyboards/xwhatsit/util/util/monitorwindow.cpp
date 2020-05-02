#include "monitorwindow.h"
#include "ui_monitorwindow.h"
#include <QGraphicsView>
#include <QPainter>
#include <QFile>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>

MonitorWindow::MonitorWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MonitorWindow)
{
    ui->setupUi(this);
}

MonitorWindow::~MonitorWindow()
{
    delete ui;
}

void MonitorWindow::loadLayout(const QString &name)
{
    QFile f(name);
    f.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = f.readAll();
    f.close();
    QJsonParseError errorPtr;
    doc = QJsonDocument::fromJson(data, &errorPtr);
    if (doc.isNull()) {
        return;
    }
    ui->layoutSel->clear();
    QJsonObject root = doc.object();
    QJsonValue layoutsValue = root.value("layouts");
    QJsonObject layouts = layoutsValue.toObject();
    for (const auto &i : layouts.keys())
    {
        ui->layoutSel->addItem(i);
    }
}

void MonitorWindow::paintEvent(QPaintEvent *event)
{

    QJsonArray keys = doc.object().value("layouts").toObject().value(ui->layoutSel->currentText()).toObject().value("layout").toArray();
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(QPen(Qt::black));
    painter.setBrush(QBrush(QColor("#FFFFFF")));
    int xadd = ui->layoutSel->geometry().x();
    int yadd = ui->last_label->geometry().y() + ui->last_label->geometry().height() + ui->layoutSel->geometry().y();
    int maxy = 0;
    int maxx = 0;
    for (const auto i : keys)
    {
        double scale = 32;
        int x = static_cast<int>(scale * i.toObject().value("x").toDouble() + 0.5);
        int y = static_cast<int>(scale * i.toObject().value("y").toDouble() + 0.5);
        int w = static_cast<int>(scale * 1 + 0.5);
        int h = static_cast<int>(scale * 1 + 0.5);
        if (!i.toObject().value("w").isUndefined())
        {
            w = static_cast<int>(scale * i.toObject().value("w").toDouble() + 0.5);
        }
        if (!i.toObject().value("h").isUndefined())
        {
            h = static_cast<int>(scale * i.toObject().value("h").toDouble() + 0.5);
        }
        y += yadd;
        x += xadd;
        painter.drawRect(x, y, w, h);
        if (y+h > maxy)
        {
            maxy = y + h;
        }
        if (x+w > maxx)
        {
            maxx = x + w;
        }
    }
    int minwidth1 = ui->layoutSel->geometry().width() + 2 * ui->layoutSel->geometry().x();
    int minwidth2 = ui->layoutSel->geometry().x() + maxx;
    int minwidth = (minwidth1 > minwidth2) ? minwidth1 : minwidth2;
    int minheight = maxy + ui->layoutSel->geometry().y();
    this->setFixedSize(minwidth, minheight);
}

void MonitorWindow::on_layoutSel_activated(const QString &arg1)
{
    Q_UNUSED(arg1);
    this->repaint();
}
