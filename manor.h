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
    explicit Manor(QWidget* parent = nullptr);
    ~Manor();

private slots:
    void add_property();
    void add_unit();
    void add_tenant();

private:
    void setup_properties_combo();
    void setup_units_list();
    void setup_tenants_table();
    void setup_actions();

private:
    Ui::Manor* ui;
    QSqlTableModel* m_property_model;
    QSqlRelationalTableModel* m_unit_model;
    QSqlTableModel* m_tenant_model;
};
#endif // MANOR_H
