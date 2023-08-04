#ifndef MANOR_H
#define MANOR_H

#include <QMainWindow>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui {
class Manor;
}
QT_END_NAMESPACE

class Manor : public QMainWindow {
    Q_OBJECT

public:
    explicit Manor(QWidget* parent = nullptr);
    ~Manor();

private slots:
    void addProperty();
    void change_property(int row);
    void quitApp();
    void add_unit();
    void handle_unit_activated(const QModelIndex &idx);

private:
    Ui::Manor* ui;
    QSqlTableModel* m_property_model;
    QSqlRelationalTableModel* m_unit_model;
    QSystemTrayIcon* m_system_tray_icon;
};
#endif // MANOR_H
