#pragma once

#include <QCalendarWidget>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <QStringLiteral>
#include <QWizard>

class LeaseWizard : public QWizard {
    Q_OBJECT
public:
    explicit LeaseWizard(QWidget* parent = nullptr);
};

class PropertyPage : public QWizardPage {
    Q_OBJECT

    QComboBox* m_property_combo;

public:
    explicit PropertyPage(QWidget* parent = nullptr);
};

class UnitPage : public QWizardPage {
    Q_OBJECT

    QComboBox* m_units_combo;
    QLabel* m_selected_property;

public:
    explicit UnitPage(QWidget* parent = nullptr);

    // QWizardPage interface
    void initializePage() override;
};

class TenantPage : public QWizardPage {
    Q_OBJECT

    QComboBox* m_tenants_combo;
    QLabel* m_selected_property;
    QLabel* m_selected_unit;

public:
    explicit TenantPage(QWidget* parent = nullptr);

    // QWizardPage interface
    void initializePage() override;
};

class LeaseDetailsPage : public QWizardPage {
    Q_OBJECT

    // fields from the leases database table
    QCalendarWidget* m_start_date;
    QCalendarWidget* m_end_date;
    QLineEdit* m_rent;
    QLineEdit* m_security;

public:
    explicit LeaseDetailsPage(QWidget* parent = nullptr);

    // QWizardPage interface
    void initializePage() override;
    bool validatePage() override;
};

class FinalPage : public QWizardPage {
    Q_OBJECT

    QLabel* m_selected_property;
    QLabel* m_selected_unit;
    QLabel* m_selected_tenant;

public:
    explicit FinalPage(QWidget* parent = nullptr);

    // QWizardPage interface
    void initializePage() override;
};
