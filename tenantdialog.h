#pragma once

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class TenantDialog;
}

class TenantDialog : public QDialog {
    Q_OBJECT

public:
    explicit TenantDialog(QSqlTableModel* tenants, QWidget* parent = nullptr);
    ~TenantDialog() override;

private:
    Ui::TenantDialog* ui;
    QSqlTableModel* m_tenants_model;
};
