#pragma once

#include <QDialog>
#include <QSqlTableModel>

namespace Ui {
class PropertyDialog;
}

class PropertyDialog : public QDialog {
    Q_OBJECT

public:
    PropertyDialog(QSqlTableModel* properties, QWidget* parent = nullptr);
    ~PropertyDialog() override;

private:
    Ui::PropertyDialog* ui;
    QSqlTableModel* m_property_model;
};
