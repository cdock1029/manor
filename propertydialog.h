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
    PropertyDialog(QSqlTableModel* properties, QWidget* parent = nullptr);
    ~PropertyDialog();

private slots:
    void submit();

private:
    void handleError(const QString& code);
    int addNewProperty(const QString& name);

private:
    Ui::PropertyDialog* ui;
    QSqlTableModel* m_PropertyModel;
    static inline const QString SQLITE_CONSTRAINT_UNIQUE = "2067";
};

#endif // PROPERTYDIALOG_H
