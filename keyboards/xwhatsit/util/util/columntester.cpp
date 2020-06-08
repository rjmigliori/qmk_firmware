#include "columntester.h"
#include "ui_columntester.h"
#include <QSizePolicy>
#include <stdio.h>
#include <QMenu>
#include <QAction>

ColumnTester::ColumnTester(HidThread &thread, std::string path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColumnTester),
    thread(thread),
    path(path)
{
    ui->setupUi(this);
    int i;
    for (i=1;i<24;i++)
    {
        QPushButton *button = new QPushButton(QString::number(i), this);
        buttons.push_back(button);
        button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        button->setMinimumSize(20, 0);
        button->setCheckable(true);
        ui->horizontalLayout->addWidget(button);
        connect(button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    }
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowContextMenu(const QPoint &)));
}

void ColumnTester::ShowContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this);
    QAction action1("Close", this);
    connect(&action1, SIGNAL(triggered()), this, SLOT(close()));
    contextMenu.addAction(&action1);
    contextMenu.exec(mapToGlobal(pos));
}

ColumnTester::~ColumnTester()
{
    for(auto const& value: buttons) {
        delete value;
    }

    delete ui;
}

void ColumnTester::onButtonClicked()
{
    uint32_t data = 0;
    for (int i=0;i<23;i++)
    {
        if (buttons[i]->isChecked())
        {
            data |= 1 << i;
        }
    }
    thread.shiftData(path, data);
}
