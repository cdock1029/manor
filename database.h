#ifndef DATABASE_H
#define DATABASE_H

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

static bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Cannot open database", "Needs SQLite support.", QMessageBox::Cancel);
        return false;
    }

    QSqlQuery query;

    QSqlDatabase::database().transaction();

    query.exec("DROP TABLE IF EXISTS properties");
    query.exec("CREATE TABLE IF NOT EXISTS properties ("
               "id      INTEGER,"
               "name	TEXT NOT NULL UNIQUE COLLATE NOCASE,"
               "PRIMARY KEY(id))");
    query.exec("INSERT INTO properties (name) VALUES ('Columbiana Manor')");
    query.exec("INSERT INTO properties (name) VALUES ('Westchester Commons')");
    query.exec("INSERT INTO properties (name) VALUES ('ACME Acres')");

    QSqlDatabase::database().commit();
    return true;
}

#endif // DATABASE_H
