#ifndef MANOR_H
#define MANOR_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class Manor; }
QT_END_NAMESPACE

class Manor : public QMainWindow
{
    Q_OBJECT

public:
    Manor(QWidget *parent = nullptr);
    ~Manor();

private:
    Ui::Manor *ui;
};
#endif // MANOR_H
