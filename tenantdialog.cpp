#include "tenantdialog.h"
#include "database.h"
#include "shared.h"
#include "ui_tenantdialog.h"
#include <QLineEdit>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

TenantDialog::TenantDialog(QSqlTableModel* tenants, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::TenantDialog)
    , m_tenants_model(tenants)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    connect(ui->button_box, &QDialogButtonBox::accepted, this, [this]() {
        QString first, middle, last;

        first = { ui->line_edit_first->text().trimmed() };
        middle = { ui->line_edit_middle->text().trimmed() };
        last = { ui->line_edit_last->text().trimmed() };

        if (first.isEmpty() or last.isEmpty()) {
            QMessageBox::warning(this, "Tenant error", "First and Last name are required");
        } else {
            auto record = m_tenants_model->record();
            record.setGenerated(Db::TENANT_ID, false);
            record.setValue(Db::TENANT_FIRST, first);
            if (middle.isEmpty()) {
                record.setGenerated(Db::TENANT_MIDDLE, false);
            } else {
                record.setValue(Db::TENANT_MIDDLE, middle);
            }

            record.setValue(Db::TENANT_LAST, last);
            record.setNull(Db::TENANT_EMAIL);
            record.setNull(Db::TENANT_PHONE);
            m_tenants_model->insertRecord(-1, record);
            bool submitted = m_tenants_model->submitAll();
            auto new_id = m_tenants_model->query().lastInsertId().toInt();
            if (submitted && new_id > 0) {
                qDebug() << "tenant created, id: " << new_id;
                accept();
            } else {
                m_tenants_model->revertAll();
                ui->line_edit_first->clear();
                ui->line_edit_middle->clear();
                ui->line_edit_last->clear();
                auto err = m_tenants_model->lastError();
                qDebug() << "tenants submit error: " << err;
                Shared::handle_error(this, err.nativeErrorCode());
            }
        }
    });
}

TenantDialog::~TenantDialog()
{
    delete ui;
}
