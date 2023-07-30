#ifndef DATABASE_H
#define DATABASE_H

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace Db {


inline bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Cannot open database", "Needs SQLite support.", QMessageBox::Cancel);
        return false;
    }
    QVariantList names{"Columbiana Manor", "Westchester Commons", "Acme Acres"};
    // QVariantList pids{1, 2, 3};
    QVariantList acme_units{
        "A100",
        "B100",
        "C100"
    };
    QVariantList units{
        "31-101",
        "31-102",
        "31-103",
        "31-104",
        "31-105",
        "31-106",
        "31-107",
        "31-108",
        "31-110",
        "31-111",
        "31-112",
        "31-113",
        "31-114",
        "31-115",
        "31-116",
        "31-117",
        "31-118",
        "31-201",
        "31-202",
        "31-203",
        "31-204",
        "31-205",
        "31-206",
        "31-207",
        "31-208",
        "31-209",
        "31-210",
        "31-211",
        "31-212",
        "31-213",
        "31-214",
        "31-215",
        "31-216",
        "31-217",
        "31-218",
        "31-301",
        "31-302",
        "31-303",
        "31-304"
    };


    QSqlQuery properties_query;

    properties_query.exec("DROP TABLE IF EXISTS properties");
    properties_query.exec("CREATE TABLE IF NOT EXISTS properties ("
               "id      INTEGER,"
               "name	TEXT NOT NULL UNIQUE COLLATE NOCASE,"
               "PRIMARY KEY(id))");

    properties_query.prepare("INSERT INTO properties (name) VALUES (?)");
    // properties_query.addBindValue(pids);
    properties_query.addBindValue(names);

    if (!properties_query.execBatch()) {
        qDebug() << "error properties batch: " << properties_query.lastError();
        return false;
    }

    QSqlQuery units_query;

    units_query.exec("DROP TABLE IF EXISTS units");
    units_query.exec("CREATE TABLE IF NOT EXISTS units ("
                    "id	            INTEGER,"
                    "name	        TEXT NOT NULL COLLATE NOCASE,"
                    "property_id    INTEGER,"
                    "UNIQUE(property_id,name),"
                    "PRIMARY KEY(id),"
                    "FOREIGN KEY(property_id) REFERENCES properties(id) ON Delete cascade)");

    units_query.prepare("INSERT INTO units (name, property_id) VALUES (?, 1)");
    units_query.addBindValue(units);

    if (!units_query.execBatch()) {
        qDebug() << "error units setup batch: " << units_query.lastError();
        return false;
    }

    units_query.prepare("INSERT INTO units (name, property_id) VALUES (?, 3)");
    units_query.addBindValue(acme_units);
    if (!units_query.execBatch()) {
        qDebug() << "error acme units setup batch: " << units_query.lastError();
        return false;
    }

    return true;

}
}

#endif // DATABASE_H
