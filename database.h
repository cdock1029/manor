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
    QVariantList names { "Columbiana Manor", "Westchester Commons", "Acme Acres" };
    // QVariantList pids{1, 2, 3};
    QVariantList acme_units {
        "C100",
        "A100",
        "B100"
    };
    QVariantList units {
        "31-116",
        "31-117",
        "31-118",
        "31-201",
        "31-202",
        "31-203",
        "31-204",
        "31-205",
        "31-103",
        "31-104",
        "31-105",
        "31-106",
        "31-107",
        "31-108",
        "31-101",
        "31-102",
        "31-110",
        "31-111",
        "31-112",
        "31-113",
        "31-114",
        "31-115",
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

    QVariantList commons_units {
        "4881-1",
        "4881-2",
        "4881-3",
        "4881-4",
        "4881-5",
        "4881-6",
        "4883-1",
        "4883-2",
        "4883-3",
        "4883-4",
        "4883-5",
        "4883-6",
        "4885-1",
        "4885-2",
        "4885-3",
        "4885-4",
        "4885-5",
        "4887-1",
        "4887-2",
        "4887-3",
        "4887-4",
        "4887-5",
        "4887-6"
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

    units_query.prepare("insert into units (name, property_id) values (?, 2)");
    units_query.addBindValue(commons_units);
    if (!units_query.execBatch()) {
        qDebug() << "error commons units setup batch: " << units_query.lastError();
        return false;
    }

    QSqlQuery tenants;

    tenants.exec("DROP TABLE IF EXISTS tenants");
    tenants.exec("CREATE TABLE IF NOT EXISTS tenants ("
                 "id INTEGER,"
                 "first	TEXT NOT NULL COLLATE NOCASE,"
                 "middle TEXT NOT NULL DEFAULT '' COLLATE NOCASE,"
                 "last	TEXT NOT NULL COLLATE NOCASE,"
                 "email	TEXT UNIQUE COLLATE NOCASE,"
                 "phone	TEXT,"
                 "UNIQUE(last, first, middle),"
                 "PRIMARY KEY(id))");
    // QString empty = QString();
    tenants.prepare("insert into tenants (first, middle, last) values (?, ?, ?)");
    tenants.addBindValue(QVariantList { "Mona", "Saoirse", "Freya", "Amanda", "Conor" });
    tenants.addBindValue(QVariantList { "Chungus", "Sersh", "Pipsqueak", "Emma", "" });
    QString last { "Dockry" };
    tenants.addBindValue(QVariantList { last, last, last, last, last });
    // tenants.addBindValue(QVariantList{empty, empty, empty, empty, empty});
    if (!tenants.execBatch()) {
        qDebug() << "error tenants setup batch: " << tenants.lastError();
        return false;
    }

    QSqlQuery leases;
    bool valid;
    valid = leases.exec("DROP TABLE IF EXISTS leases");
    valid = valid && leases.exec("CREATE TABLE IF NOT EXISTS leases ("
                                 "id	        INTEGER,"
                                 "start	    TEXT NOT NULL CHECK(date(start) = start),"
                                 "end	    TEXT NOT NULL CHECK(date(end) = end),"
                                 "rent	    NUMERIC NOT NULL CHECK(rent > 0),"
                                 "security	NUMERIC,"
                                 "unit_id	INTEGER NOT NULL,"
                                 "tenant_id	INTEGER NOT NULL,"
                                 "FOREIGN KEY(tenant_id) REFERENCES tenants(id) on delete cascade,"
                                 "FOREIGN KEY(unit_id) REFERENCES units(id) on delete cascade,"
                                 "PRIMARY KEY(id))");
    if (!valid) {
        qDebug() << "error leases setup: " << leases.lastError();
        return false;
    }

    return true;
}
}

#endif // DATABASE_H
