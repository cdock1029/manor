#pragma once

#include <QMainWindow>
#include <QPointer>
#include <QSqlRelationalTableModel>
#include <QSqlTableModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class Manor;
}
QT_END_NAMESPACE

class Manor : public QMainWindow { // NOLINT(*-special-member-functions)
    Q_OBJECT

    Ui::Manor* ui;
    QPointer<QSqlTableModel> m_property_model;
    QPointer<QSqlTableModel> m_tenant_model;
    QPointer<QSqlQueryModel> m_active_leases_model;

public:
    explicit Manor(QWidget* parent = nullptr);
    ~Manor() override;

private:
    Q_SLOT
    void add_tenant();

    void setup_stack();
    void setup_property_tabs(int active_tab = 0);
    void setup_tenants_table();
    void setup_actions();
};
