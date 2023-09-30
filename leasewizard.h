#pragma once

#include <QComboBox>
#include <QLabel>
#include <QPointer>
#include <QStringLiteral>
#include <QWizard>

class LeaseWizard : public QWizard {
    Q_OBJECT
public:
    explicit LeaseWizard(QWidget* parent = nullptr);
    static const QString PROPERTY_FIELD;
    static const QString UNIT_FIELD;
};

class PropertyPage : public QWizardPage {
    Q_OBJECT
public:
    explicit PropertyPage(QWidget* parent = nullptr);

private:
    QPointer<QComboBox> m_properties;
};

class UnitPage : public QWizardPage {
    Q_OBJECT
public:
    explicit UnitPage(QWidget* parent = nullptr);

private:
    QPointer<QComboBox> m_units;
    QPointer<QLabel> m_selected_property;

    // QWizardPage interface
public:
    void initializePage() override;
};

class FinalPage : public QWizardPage {
    Q_OBJECT
public:
    explicit FinalPage(QWidget* parent = nullptr);

private:
    QPointer<QLabel> m_selected_property;
    QPointer<QLabel> m_selected_unit;

    // QWizardPage interface
public:
    void initializePage() override;
};
