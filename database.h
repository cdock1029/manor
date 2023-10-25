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

// active_leases view columns
inline constexpr int ACTIVE_LEASES_LEASE_ID = 8;
inline constexpr int ACTIVE_LEASES_PROPERTY_ID = 9;

inline bool createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    // localhost
    db.setDatabaseName("cdock");

    if (!db.open()) {
        qWarning() << "db error: " << db.lastError();
        QMessageBox::critical(nullptr, "Cannot open database", "Database Error.", QMessageBox::Cancel);
        return false;
    }
    /*
    QVariantList names { "Columbiana Manor", "WT Investments" };
    QVariantList wt_units;
    wt_units << "A100"
             << "B100"
             << "C100"
             << "D200"
             << "E202"
             << "F333"
             << "B202"
             << "E111";
    QVariantList columbiana_units;
    columbiana_units << "31-116"
                     << "31-117"
                     << "31-118"
                     << "31-201"
                     << "31-202"
                     << "31-203"
                     << "31-204"
                     << "31-205"
                     << "31-103"
                     << "31-104"
                     << "31-105"
                     << "31-106"
                     << "31-107"
                     << "31-108"
                     << "31-101"
                     << "31-102"
                     << "31-110"
                     << "31-111"
                     << "31-112"
                     << "31-113"
                     << "31-114"
                     << "31-115"
                     << "31-206"
                     << "31-207"
                     << "31-208"
                     << "31-209"
                     << "31-210"
                     << "31-211"
                     << "31-212"
                     << "31-213"
                     << "31-214"
                     << "31-215"
                     << "31-216"
                     << "31-217"
                     << "31-218"
                     << "31-301"
                     << "31-302"
                     << "31-303"
                     << "31-304";

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
    units_query.addBindValue(columbiana_units);

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
                     "rent	        money NOT NULL CHECK(rent > 0::money),"
                     "security	    money NOT NULL DEFAULT 0::money,"
                     "unit_id	    INTEGER references units(id) on delete cascade,"
                     "tenant_id	    INTEGER references tenants(id) on delete cascade,"
                     "active        boolean NOT NULL default false,"
                     "PRIMARY KEY(id))")) {
        qDebug() << "error leases setup: " << leases.lastError();
        return false;
    }
    if (!leases.exec("CREATE UNIQUE INDEX IF NOT EXISTS leases_active_idx ON leases (active) WHERE (active = true)")) {
        qDebug() << "error leases active index: " << leases.lastError();
        return false;
    }
    if (!leases.exec("INSERT INTO leases (start_date, end_date, rent, security, unit_id, tenant_id, active) "
                     "VALUES ('2021-01-01', '2021-12-31', 1000, 1000, 1, 1, true)")) {
        qDebug() << "error leases insert: " << leases.lastError();
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
                           "txn_type_id     INTEGER references txn_types(id) on delete cascade,"
                           "PRIMARY KEY(id))")) {
        qDebug() << "error transactions setup: " << transactions.lastError();
        return false;
    }

    QSqlQuery active_leases;
    active_leases.exec("drop view if exists active_leases");
    if (!active_leases.exec("CREATE VIEW active_leases as "
                            "SELECT u.id AS unit_id,"
                            "u.name AS unit,"
                            "l.start_date,"
                            "l.end_date,"
                            "l.rent,"
                            "l.security,"
                            "t.last,"
                            "t.first,"
                            "l.id AS lease_id,"
                            "u.property_id "
                            "FROM units u "
                            "LEFT JOIN leases l ON u.id = l.unit_id AND l.active = true "
                            "LEFT JOIN tenants t ON t.id = l.tenant_id "
                            "ORDER BY u.property_id, u.name")) {
        qDebug() << "error active_lease view setup: " << active_leases.lastError();
        return false;
    }
    */
    return true;
}
}
