#ifndef MANOR_H
#define MANOR_H

#include <QMainWindow>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class Manor;
}
QT_END_NAMESPACE

class Manor : public QMainWindow {
    Q_OBJECT

public:
    Manor(const QString& propertyTable, QWidget* parent = nullptr);
    ~Manor();

private slots:
    void addProperty();
    void quitApp();

private:
    Ui::Manor* ui;
    QSqlTableModel* m_PropertyModel;
};
#endif // MANOR_H
