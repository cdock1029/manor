#ifndef LEASEWIZARD_H
#define LEASEWIZARD_H

#include <QComboBox>
#include <QLabel>
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
    QComboBox* m_properties;
};

class UnitPage : public QWizardPage {
    Q_OBJECT
public:
    explicit UnitPage(QWidget* parent = nullptr);

private:
    QComboBox* m_units;
    QLabel* m_selected_property;

    // QWizardPage interface
public:
    void initializePage() override;
};

class FinalPage : public QWizardPage {
    Q_OBJECT
public:
    explicit FinalPage(QWidget* parent = nullptr);

private:
    QLabel* m_selected_property;
    QLabel* m_selected_unit;

    // QWizardPage interface
public:
    void initializePage() override;
};

#endif // LEASEWIZARD_H
