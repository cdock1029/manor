#include "manor.h"
#include "./ui_manor.h"
#include <QListWidget>

Manor::Manor(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::Manor)
{
    ui->setupUi(this);
    connect(ui->action_Quit, &QAction::triggered, this, &Manor::quitApp);
    connect(ui->listWidget, &QListWidget::currentItemChanged, this, &Manor::updateLabel);
}

Manor::~Manor()
{
    delete ui;
}

void Manor::quitApp()
{
    this->close();
}

void Manor::updateLabel(QListWidgetItem* current, QListWidgetItem* previous)
{
    ui->labelT1P1->setText(current->text());
}
