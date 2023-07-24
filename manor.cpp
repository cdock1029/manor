#include "manor.h"
#include "./ui_manor.h"

Manor::Manor(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Manor)
{
    ui->setupUi(this);
}

Manor::~Manor()
{
    delete ui;
}

