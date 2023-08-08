#include "leasewizard.h"

#include <QGridLayout>

const QString LeaseWizard::PROPERTY_FIELD = QStringLiteral(u"property_selection");
const QString LeaseWizard::UNIT_FIELD = QStringLiteral(u"unit_selection");

LeaseWizard::LeaseWizard(QWidget* parent)
    : QWizard(parent)
{
    setWindowTitle("New Lease");
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/napoleon1.png"));
    setAttribute(Qt::WA_DeleteOnClose);
    setOption(NoBackButtonOnStartPage);

    setDefaultProperty("QComboBox", "currentData", SIGNAL(currentIndexChanged(int)));

    addPage(new PropertyPage);
    addPage(new UnitPage);
    addPage(new FinalPage);
}

PropertyPage::PropertyPage(QWidget* parent)
    : QWizardPage(parent)
{
    setTitle("Property selection");
    setSubTitle("Select a property for the lease");

    m_properties = new QComboBox;
    m_properties->setPlaceholderText("Choose property");
    std::vector<ComboPair> items = { { "Acme acres", 100 }, { "Middle Earth", 200 }, { "Rainbow bridge", 300 } };
    for (auto& item : items) {
        m_properties->insertItem((int)items.size(), item.name, QVariant::fromValue(item));
    }

    registerField(LeaseWizard::PROPERTY_FIELD + "*", m_properties);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(m_properties);
    setLayout(layout);
}

UnitPage::UnitPage(QWidget* parent)
    : QWizardPage(parent)
{
    setTitle("Unit selection");
    setSubTitle("Select the unit for the lease");

    m_selected_property = new QLabel;

    m_units = new QComboBox;
    m_units->setPlaceholderText("Choose unit");
    std::vector<ComboPair> items = { { "A-101", 111 }, { "B-202", 222 }, { "C-303", 333 } };
    for (auto& item : items) {
        m_units->insertItem((int)items.size(), item.name, QVariant::fromValue(item));
    }

    registerField(LeaseWizard::UNIT_FIELD + "*", m_units);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(m_selected_property);
    layout->addWidget(m_units);
    setLayout(layout);
}

void UnitPage::initializePage()
{
    ComboPair property = field(LeaseWizard::PROPERTY_FIELD).value<ComboPair>();
    m_selected_property->setText(property.name);
}

FinalPage::FinalPage(QWidget* parent)
    : QWizardPage(parent)
{
    setTitle("Summary");
    setSubTitle("Confirm details of lease");

    m_selected_property = new QLabel;
    m_selected_unit = new QLabel;

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(m_selected_property);
    layout->addWidget(m_selected_unit);
    setLayout(layout);
}

void FinalPage::initializePage()
{
    auto property = field(LeaseWizard::PROPERTY_FIELD).value<ComboPair>();
    auto unit = field(LeaseWizard::UNIT_FIELD).value<ComboPair>();
    m_selected_property->setText(property.name);
    m_selected_unit->setText(unit.name);
}
