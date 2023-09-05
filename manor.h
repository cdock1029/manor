#ifndef MANOR_H
#define MANOR_H

#include <QMainWindow>
#include <QPointer>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class Manor;
}
QT_END_NAMESPACE

class Manor : public QMainWindow { // NOLINT(cppcoreguidelines-special-member-functions)
    Q_OBJECT

public:
    explicit Manor(QWidget* parent = nullptr);
    ~Manor() override;

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
    QPointer<QSqlRelationalTableModel> m_unit_model;
    QPointer<QSqlTableModel> m_property_model;
    QPointer<QSqlTableModel> m_tenant_model;
};
#endif // MANOR_H
