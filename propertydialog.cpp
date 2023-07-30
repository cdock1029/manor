#include "propertydialog.h"
#include "ui_propertydialog.h"
#include <QDialogButtonBox>
#include <QMessageBox>

PropertyDialog::PropertyDialog(QSqlTableModel* properties, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::PropertyDialog)
    , m_PropertyModel(properties)
{
    ui->setupUi(this);

    connect(ui->propertyButtonBox, &QDialogButtonBox::accepted, this, &PropertyDialog::submit);
}

PropertyDialog::~PropertyDialog()
{
    delete ui;
}

void PropertyDialog::submit()
{
    QString propertyName = ui->propertyNameEdit->text();

    if (propertyName.trimmed().isEmpty()) {
        QMessageBox::information(this, "Save property", "Property name is required.");
    } else {
        if (addNewProperty(propertyName) > 0) {
            accept();
        }
    }
}

int PropertyDialog::addNewProperty(const QString& name)
{
    auto record = m_PropertyModel->record();

    record.setGenerated(0, false);
    record.setValue(1, QVariant(name.trimmed()));

    bool inserted = m_PropertyModel->insertRecord(-1, record);
    bool submitted = m_PropertyModel->submitAll();
    if (!submitted) {
        m_PropertyModel->revertAll();
        ui->propertyNameEdit->clear();
        auto err = m_PropertyModel->lastError();
        qDebug() << "property submit error: " << err;
        handleError(err.nativeErrorCode());
        return -1;
    }
    int id = m_PropertyModel->query().lastInsertId().toInt();

    qInfo() << "inserted:" << inserted << ", submitted:" << submitted << ", id:" << id;

    return id;
}

void PropertyDialog::handleError(const QString& code)
{
    if (code == SQLITE_CONSTRAINT_UNIQUE) {
        QMessageBox::warning(this, "Save property", "Property name must be unique. That one already exists.");
    } else {
        QMessageBox::warning(this, "Save property", "Unspecified error.");
    }
}
