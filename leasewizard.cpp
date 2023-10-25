#include "leasewizard.h"
#include <QGridLayout>
#include <QSqlField>
#include <QSqlQueryModel>
#include <QSqlRecord>

using namespace Qt::StringLiterals;

const QString PROPERTY_FIELD = u"property_selection"_s;
const QString UNIT_FIELD = u"unit_selection"_s;
const QString TENANT_FIELD = u"tenant_selection"_s;
const QString START_DATE = u"start_date"_s;
const QString END_DATE = u"end_date"_s;
const QString RENT = u"rent"_s;
const QString SECURITY = u"security"_s;

struct ComboPair {
    QString name {};
    int id {};
};
Q_DECLARE_METATYPE(ComboPair)

LeaseWizard::LeaseWizard(QWidget* parent)
    : QWizard(parent)
{
    setWindowTitle("New Lease");
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/napoleon1.png"));
    setAttribute(Qt::WA_DeleteOnClose);
    setOption(NoBackButtonOnStartPage);

    setDefaultProperty("QComboBox", "currentData", SIGNAL(currentIndexChanged(int)));
    setDefaultProperty("QCalendarWidget", "selectedDate", SIGNAL(clicked(QDate)));

    addPage(new PropertyPage { this });
    addPage(new UnitPage { this });
    addPage(new TenantPage { this });
    addPage(new LeaseDetailsPage { this });
    addPage(new FinalPage { this });
}

PropertyPage::PropertyPage(QWidget* parent)
    : QWizardPage(parent)
{
    setTitle("Property selection");
    setSubTitle("Select a property for the lease");

    m_property_combo.setPlaceholderText("Choose property");

    QSqlQueryModel properties;
    properties.setQuery(u"SELECT id, name FROM properties order by name asc"_s);

    const int count = properties.rowCount();
    for (int i = 0; i < count; ++i) {
        auto id = properties.record(i).value(0).toInt();
        auto name = properties.record(i).value(1).toString();
        m_property_combo.insertItem(count, name, QVariant::fromValue(ComboPair { name, id }));
    }

    registerField(PROPERTY_FIELD + "*", &m_property_combo);

    auto* layout = new QGridLayout { this };
    layout->addWidget(&m_property_combo);
    setLayout(layout);
}

UnitPage::UnitPage(QWidget* parent)
    : QWizardPage(parent)
{
    setTitle("Unit selection");
    setSubTitle("Select the unit for the lease");

    m_units_combo.setPlaceholderText("Choose unit");

    registerField(UNIT_FIELD + "*", &m_units_combo);

    auto* layout = new QGridLayout { this };
    layout->addWidget(&m_selected_property);
    layout->addWidget(&m_units_combo);
    setLayout(layout);
}

void UnitPage::initializePage()
{
    auto property = field(PROPERTY_FIELD).value<ComboPair>();

    QSqlQueryModel units;
    units.setQuery(u"SELECT id, name FROM units where property_id = %1 order by name asc"_s.arg(property.id));

    // clear the combo box for when using the back button
    m_units_combo.clear();
    for (int i = 0; i < units.rowCount(); ++i) {
        auto id = units.record(i).value(0).toInt();
        auto name = units.record(i).value(1).toString();
        m_units_combo.insertItem(units.rowCount(), name, QVariant::fromValue(ComboPair { name, id }));
    }
    m_selected_property.setText(property.name);
}

TenantPage::TenantPage(QWidget* parent)
    : QWizardPage(parent)
{
    setTitle("Tenant selecion");
    setSubTitle("Select tenant for the lease");

    m_tenants_combo.setPlaceholderText("Choose tenant");
    registerField(TENANT_FIELD + "*", &m_tenants_combo);
    auto* layout = new QGridLayout { this };
    layout->addWidget(&m_selected_property);
    layout->addWidget(&m_selected_unit);
    layout->addWidget(&m_tenants_combo);
    setLayout(layout);
}

void TenantPage::initializePage()
{
    auto property = field(PROPERTY_FIELD).value<ComboPair>();
    auto unit = field(UNIT_FIELD).value<ComboPair>();
    auto* tenants = new QSqlQueryModel { this };
    tenants->setQuery(u"SELECT id, first, middle, last from tenants order by last asc"_s);
    m_tenants_combo.clear();
    for (int i = 0; i < tenants->rowCount(); ++i) {
        auto id = tenants->record(i).value(0).toInt();
        auto first = tenants->record(i).value(1).toString();
        auto middle = tenants->record(i).value(2).toString();
        auto last = tenants->record(i).value(3).toString();
        auto name = middle.isEmpty() ? QString("%1 %2").arg(first, last) : QString("%1 %2 %3").arg(first, middle, last);
        m_tenants_combo.insertItem(tenants->rowCount(), name, QVariant::fromValue(ComboPair { name, id }));
    }
    m_selected_property.setText(property.name);
    m_selected_unit.setText(unit.name);
}

LeaseDetailsPage::LeaseDetailsPage(QWidget* parent)
    : QWizardPage(parent)
    , m_start_date { new QCalendarWidget { this } }
    , m_end_date { new QCalendarWidget { this } }
    , m_rent { new QLineEdit { this } }
    , m_security { new QLineEdit { this } }
{
    setTitle("Lease details");
    setSubTitle("Enter lease details");

    m_start_date->setToolTip(u"Start Date"_s);
    m_start_date->setSelectedDate(QDate::currentDate());

    auto date_handler = [this](QDate start_date) {
        const QPointer<QCalendarWidget> calendar = qobject_cast<QCalendarWidget*>(sender());
        const auto index = senderSignalIndex();
        qInfo() << "handler index:" << index << ",Start date:" << start_date;
        m_end_date->setSelectedDate(start_date.addYears(1));
        m_end_date->setMinimumDate(start_date.addMonths(1));
    };

    connect(m_start_date, &QCalendarWidget::clicked, this, date_handler);
    connect(m_start_date, &QCalendarWidget::activated, this, date_handler);

    m_end_date->setToolTip(u"End Date"_s);
    m_end_date->setSelectedDate(QDate::currentDate().addYears(1));

    auto* validator = new QDoubleValidator { 0.0, MAX_MONTHLY_RENT, 2, this };
    m_rent->setValidator(validator);
    m_security->setValidator(validator);

    registerField(START_DATE + "*", m_start_date);
    registerField(END_DATE + "*", m_end_date);
    registerField(RENT + "*", m_rent);
    registerField(SECURITY + "*", m_security);

    auto layout = QPointer { new QGridLayout { this } };

    layout->addWidget(new QLabel { "Start Date", this }, 0, 0);
    layout->addWidget(m_start_date, 1, 0);

    layout->addWidget(new QLabel { "End Date", this }, 0, 1);
    layout->addWidget(m_end_date, 1, 1);

    layout->addWidget(new QLabel { "Enter monthly rent", this }, 2, 0);
    layout->addWidget(m_rent, 2, 1);

    layout->addWidget(new QLabel { "Enter security deposit", this }, 3, 0);
    layout->addWidget(m_security, 3, 1);

    setLayout(layout);
}

void LeaseDetailsPage::initializePage()
{
}

bool LeaseDetailsPage::validatePage()
{
    return true;
}

FinalPage::FinalPage(QWidget* parent)
    : QWizardPage(parent)
{
    setTitle("Summary");
    setSubTitle("Confirm details of lease");

    auto layout = QPointer { new QGridLayout { this } };
    layout->addWidget(&m_selected_property);
    layout->addWidget(&m_selected_unit);
    layout->addWidget(&m_selected_tenant);
    layout->addWidget(&m_start);
    layout->addWidget(&m_end);
    layout->addWidget(&m_rent);
    layout->addWidget(&m_security);
    setLayout(layout);
}

void FinalPage::initializePage()
{
    auto property = field(PROPERTY_FIELD).value<ComboPair>();
    auto unit = field(UNIT_FIELD).value<ComboPair>();
    auto tenant = field(TENANT_FIELD).value<ComboPair>();
    auto start = field(START_DATE).toDate();
    auto end = field(END_DATE).toDate();
    auto rent = field(RENT).toDouble();
    auto security = field(SECURITY).toDouble();
    m_selected_property.setText(QString("Property: %1").arg(property.name));
    m_selected_unit.setText(QString("Unit: %1").arg(unit.name));
    m_selected_tenant.setText(QString("Tenant: %1").arg(tenant.name));
    m_start.setText(QString("Start date: %1").arg(start.toString()));
    m_end.setText(QString("End date: %1").arg(end.toString()));
    m_rent.setText(QString("Rent: %1").arg(rent));
    m_security.setText(QString("Security deposit: %1").arg(security));
}
