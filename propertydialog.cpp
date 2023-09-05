#include "propertydialog.h"
#include "shared.h"
#include "ui_propertydialog.h"
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

PropertyDialog::PropertyDialog(QSqlTableModel* properties, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::PropertyDialog)
    , m_property_model(properties)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    connect(ui->property_btn_box, &QDialogButtonBox::accepted, this, [this]() {
        QString propertyName = ui->propertyNameEdit->text();

        if (propertyName.trimmed().isEmpty()) {
            QMessageBox::warning(this, "Save property", "Property name is required.");
        } else {
            auto record = m_property_model->record();

            record.setGenerated(0, false);
            record.setValue(1, propertyName.trimmed());

            m_property_model->insertRecord(-1, record);
            bool submitted = m_property_model->submitAll();

            if (submitted and m_property_model->query().lastInsertId().toInt() > 0) {
                accept();
            } else {
                m_property_model->revertAll();
                ui->propertyNameEdit->clear();
                auto err = m_property_model->lastError();
                qDebug() << "property submit error: " << err;
                Shared::handle_error(this, err.nativeErrorCode());
            }
        }
    });
}

PropertyDialog::~PropertyDialog()
{
    delete ui;
}
