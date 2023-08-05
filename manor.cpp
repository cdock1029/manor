#include "manor.h"
#include "./ui_manor.h"
#include "propertydialog.h"
#include "shared.h"
#include "tenantdialog.h"
#include <QInputDialog>
#include <QItemSelection>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTableView>
// #include <QListView>
// #include <QSqlRelation>
// #include <QtLogging>

Manor::Manor(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::Manor)
{
    m_unit_model = new QSqlRelationalTableModel(this);
    m_unit_model->setTable("units");
    m_unit_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_unit_model->setRelation(2, QSqlRelation("properties", "id", "name"));

    m_property_model = m_unit_model->relationModel(2);
    m_property_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    m_unit_model->setSort(1, Qt::AscendingOrder);
    m_unit_model->setFilter("property_id = -1");
    m_unit_model->select();

    m_tenant_model = new QSqlTableModel(this);
    m_tenant_model->setTable("tenants");
    m_tenant_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_tenant_model->setSort(3, Qt::AscendingOrder);
    m_tenant_model->select();

    ui->setupUi(this);

    setup_properties_combo();
    setup_units_list();
    setup_tenants_table();
    setup_actions();
}

Manor::~Manor()
{
    delete ui;
}

void Manor::setup_properties_combo()
{
    auto properties_combo = ui->properties_combo;
    properties_combo->setModel(m_property_model);
    properties_combo->setModelColumn(1);
    properties_combo->setPlaceholderText("Select a Property");
    properties_combo->setCurrentIndex(-1);

    connect(properties_combo, &QComboBox::currentIndexChanged, this, [this](int row) {
        auto idx = m_property_model->index(row, 0);
        m_unit_model->setFilter("property_id = " + idx.data().toString());
        ui->selected_property_label->setText(idx.sibling(row, 1).data().toString());
        ui->selected_unit_label->setText(QString());
    });
}

void Manor::setup_units_list()
{
    auto units_list = ui->units_list_view;
    units_list->setViewMode(QListView::ListMode);
    units_list->setModel(m_unit_model);
    units_list->setModelColumn(1);
    units_list->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(units_list->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex& curr, [[maybe_unused]] const QModelIndex& prev) {
        ui->selected_unit_label->setText(curr.data().toString());
    });
}

void Manor::setup_tenants_table()
{
    auto tenants_table = ui->tenants_table_view;
    tenants_table->setModel(m_tenant_model);
}

void Manor::setup_actions()
{
    connect(ui->action_quit, &QAction::triggered, this, [this]() {
        this->close();
    });
    connect(ui->action_new_property, &QAction::triggered, this, &Manor::add_property);
    connect(ui->action_new_unit, &QAction::triggered, this, &Manor::add_unit);
    connect(ui->action_new_tenant, &QAction::triggered, this, &Manor::add_tenant);
}

void Manor::add_property()
{
    PropertyDialog* dialog = new PropertyDialog { m_property_model, this };
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->open();
}

void Manor::add_unit()
{
    auto combo_index = ui->properties_combo->currentIndex();
    // check if property selected
    if (combo_index >= 0) {
        // dialog for text input

        auto d = QInputDialog(this);
        d.setInputMode(QInputDialog::TextInput);
        d.setWindowTitle("New unit");
        d.setLabelText("Unit name");
        d.setOkButtonText("Save");
        d.resize(400, 200);
        if (d.exec() == QDialog::Accepted && !d.textValue().trimmed().isEmpty()) {
            auto unit = d.textValue().trimmed();
            auto id = m_property_model->index(combo_index, 0).data();
            auto record = m_unit_model->record();
            record.setGenerated(0, false);
            record.setValue(1, unit);
            record.setValue(2, id);

            bool inserted = m_unit_model->insertRecord(-1, record);
            bool submitted = m_unit_model->submitAll();
            if (!submitted) {
                m_unit_model->revertAll();
                auto err = m_unit_model->lastError();
                qDebug() << "unit submit error: " << err;
                Shared::handle_error(this, err.nativeErrorCode());
                return;
            }
            int new_id = m_unit_model->query().lastInsertId().toInt();

            qInfo() << "inserted:" << inserted << ", submitted:" << submitted << ", new id:" << new_id;
        }
    }
}

void Manor::add_tenant()
{
    TenantDialog* dialog = new TenantDialog { m_tenant_model, this };
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->open();
}
