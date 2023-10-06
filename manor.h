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

class Manor : public QMainWindow {
    Q_OBJECT

    Ui::Manor* ui;
    QPointer<QSqlRelationalTableModel> m_unit_model;
    QPointer<QSqlTableModel> m_property_model;
    QPointer<QSqlTableModel> m_tenant_model;

public:
    explicit Manor(QWidget* parent = nullptr);
    ~Manor() override;

private slots:
    void add_property();
    void add_unit();
    void add_tenant();

private:
    void setup_stack();
    void setup_property_tabs();
    void setup_tenants_table();
    void setup_actions();
};
