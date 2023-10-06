#pragma once

#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace Db {

inline constexpr int PROPERTY_ID = 0;
inline constexpr int PROPERTY_NAME = 1;

inline constexpr int UNIT_ID = 0;
inline constexpr int UNIT_NAME = 1;
inline constexpr int UNIT_PROPERTY_ID = 2;

inline constexpr int TENANT_ID = 0;
inline constexpr int TENANT_FIRST = 1;
inline constexpr int TENANT_MIDDLE = 2;
inline constexpr int TENANT_LAST = 3;
inline constexpr int TENANT_EMAIL = 4;
inline constexpr int TENANT_PHONE = 5;

inline bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("cdock");
    if (!db.open()) {
        qWarning() << "db error: " << db.lastError();
        QMessageBox::critical(nullptr, "Cannot open database", "Needs SQLite support.", QMessageBox::Cancel);
        return false;
    }
    QVariantList names { "Columbiana Manor", "WT Investments" };
    QVariantList wt_units {
        "C100",
        "A100",
        "B100",
        "D200",
        "F333",
        "E202"
    };
    QVariantList colubiana_units {
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

    QSqlQuery properties_query;

    if (!properties_query.exec("CREATE EXTENSION if not exists citext")) {
        qDebug() << "error citext: " << properties_query.lastError();
        return false;
    }
    if (!properties_query.exec("DROP TABLE IF EXISTS properties cascade")) {
        qDebug() << "error drop properties table: " << properties_query.lastError();
        return false;
    }
    if (!properties_query.exec("CREATE TABLE IF NOT EXISTS properties ("
                               "id      serial,"
                               "name	citext NOT NULL UNIQUE,"
                               "PRIMARY KEY(id))")) {
        qDebug() << "error properties setup: " << properties_query.lastError();
        return false;
    }

    properties_query.prepare("INSERT INTO properties (name) VALUES (?)");
    properties_query.addBindValue(names);

    if (!properties_query.execBatch()) {
        qDebug() << "error properties batch: " << properties_query.lastError();
        return false;
    }

    QSqlQuery units_query;

    units_query.exec("DROP TABLE IF EXISTS units cascade");
    if (!units_query.exec("CREATE TABLE IF NOT EXISTS units ("
                          "id               serial,"
                          "name             citext NOT NULL,"
                          "property_id      integer references properties(id) on delete cascade,"
                          "PRIMARY KEY(id),"
                          "UNIQUE(property_id,name))")) {
        qDebug() << "error units create table: " << units_query.lastError();
        return false;
    }

    units_query.prepare("INSERT INTO units (name, property_id) VALUES (?, 1)");
    units_query.addBindValue(colubiana_units);

    if (!units_query.execBatch()) {
        qDebug() << "error units setup batch: " << units_query.lastError();
        return false;
    }

    units_query.prepare("INSERT INTO units (name, property_id) VALUES (?, 2)");
    units_query.addBindValue(wt_units);
    if (!units_query.execBatch()) {
        qDebug() << "error acme units setup batch: " << units_query.lastError();
        return false;
    }

    QSqlQuery tenants;

    tenants.exec("DROP TABLE IF EXISTS tenants cascade");
    tenants.exec("CREATE TABLE IF NOT EXISTS tenants ("
                 "id            serial,"
                 "first	        citext NOT NULL,"
                 "middle        citext NOT NULL DEFAULT '',"
                 "last	        citext NOT NULL,"
                 "email	        citext UNIQUE,"
                 "phone	        TEXT,"
                 "UNIQUE(last, first, middle),"
                 "PRIMARY KEY(id))");
    tenants.prepare("insert into tenants (first, middle, last) values (?, ?, ?)");
    tenants.addBindValue(QVariantList { "Mona", "Saoirse", "Freya", "Amanda", "Conor" });
    tenants.addBindValue(QVariantList { "Chungus", "Sersh", "Pipsqueak", "Emma", "" });
    QString last { "Dockry" };
    tenants.addBindValue(QVariantList { last, last, last, last, last });
    if (!tenants.execBatch()) {
        qDebug() << "error tenants setup batch: " << tenants.lastError();
        return false;
    }

    QSqlQuery leases;
    if (!leases.exec("DROP TABLE IF EXISTS leases cascade")) {
        qDebug() << "error drop leases table: " << leases.lastError();
        return false;
    }
    if (!leases.exec("CREATE TABLE IF NOT EXISTS leases ("
                     "id	        serial,"
                     "start_date	DATE NOT NULL,"
                     "end_date	    DATE NOT NULL,"
                     "rent	        money NOT NULL CHECK(rent > '0'::float8::numeric::money),"
                     "security	    money,"
                     "unit_id	    INTEGER references units(id) on delete cascade,"
                     "tenant_id	    INTEGER references tenants(id) on delete cascade,"
                     "PRIMARY KEY(id))")) {
        qDebug() << "error leases setup: " << leases.lastError();
        return false;
    }

    QSqlQuery txn_types;
    txn_types.exec("DROP TABLE IF EXISTS txn_types cascade");
    if (!txn_types.exec("CREATE TABLE IF NOT EXISTS txn_types ("
                        "id	        serial,"
                        "name	    citext NOT NULL UNIQUE,"
                        "PRIMARY KEY(id))")) {
        qDebug() << "error txn_types setup: " << txn_types.lastError();
        return false;
    }

    QSqlQuery transactions;
    transactions.exec("DROP TABLE IF EXISTS transactions cascade");
    if (!transactions.exec("CREATE TABLE IF NOT EXISTS transactions ("
                           "id	            serial,"
                           "txn_date	    DATE NOT NULL,"
                           "amount	        money NOT NULL,"
                           "note	        TEXT,"
                           "lease_id	    INTEGER references leases(id) on delete cascade,"
                           "txn_type_id	INTEGER references txn_types(id) on delete cascade,"
                           "PRIMARY KEY(id))")) {
        qDebug() << "error transactions setup: " << transactions.lastError();
        return false;
    }

    return true;
}
}
