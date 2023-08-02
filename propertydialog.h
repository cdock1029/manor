#ifndef PROPERTYDIALOG_H
#define PROPERTYDIALOG_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class PropertyDialog;
}

class PropertyDialog : public QDialog {
    Q_OBJECT

public:
    explicit PropertyDialog(QSqlTableModel* properties, QWidget* parent = nullptr);
    ~PropertyDialog();

private slots:
    void submit();

private:
    int addNewProperty(const QString& name);

private:
    Ui::PropertyDialog* ui;
    QSqlTableModel* m_PropertyModel;
};

#endif // PROPERTYDIALOG_H
