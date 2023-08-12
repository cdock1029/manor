#include "leasewizard.h"

#include <QGridLayout>
#include <QSqlQueryModel>
#include <QSqlRecord>

const QString LeaseWizard::PROPERTY_FIELD = QStringLiteral(u"property_selection");
const QString LeaseWizard::UNIT_FIELD = QStringLiteral(u"unit_selection");

using ComboPair = QPair<QString, int>;

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

    QSqlQueryModel* properties = new QSqlQueryModel;
    properties->setQuery(QStringLiteral(u"SELECT id, name FROM properties order by name asc"));

    const int count = properties->rowCount();
    for (int i = 0; i < count; ++i) {
        auto id = properties->record(i).value(0).toInt();
        auto name = properties->record(i).value(1).toString();
        m_properties->insertItem(count, name, QVariant::fromValue(ComboPair { name, id }));
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

    registerField(LeaseWizard::UNIT_FIELD + "*", m_units);

    QGridLayout* layout = new QGridLayout;
    layout->addWidget(m_selected_property);
    layout->addWidget(m_units);
    setLayout(layout);
}

void UnitPage::initializePage()
{
    ComboPair property = field(LeaseWizard::PROPERTY_FIELD).value<ComboPair>();

    QSqlQueryModel* units = new QSqlQueryModel;
    auto sql = QString("SELECT id, name FROM units where property_id = %1 order by name asc").arg(property.second);
    units->setQuery(sql);

    const int count = units->rowCount();
    for (int i = 0; i < count; ++i) {
        auto id = units->record(i).value(0).toInt();
        auto name = units->record(i).value(1).toString();
        m_units->insertItem(count, name, QVariant::fromValue(ComboPair { name, id }));
    }

    m_selected_property->setText(property.first);
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
    m_selected_property->setText(property.first);
    m_selected_unit->setText(unit.first);
}
