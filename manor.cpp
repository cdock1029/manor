#include "manor.h"
#include "./ui_manor.h"
#include "propertydialog.h"
#include "shared.h"
#include <QListView>
#include <QtLogging>
#include <QSqlRelation>
#include <QInputDialog>
#include <QSystemTrayIcon>

Manor::Manor(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::Manor)
{

    m_unit_model = new QSqlRelationalTableModel(this);
    m_unit_model->setTable("units");
    m_unit_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    // id, name, property_id
    m_unit_model->setRelation(2, QSqlRelation("properties", "id", "name"));

    m_property_model = m_unit_model->relationModel(2);
    m_property_model->setEditStrategy(QSqlTableModel::OnManualSubmit);

    m_unit_model->setSort(1, Qt::AscendingOrder);
    m_unit_model->setFilter("property_id = -1");
    m_unit_model->select();

    qDebug() << "property count: " << m_property_model->rowCount()  << ", unit count: " << m_unit_model->rowCount();

    m_system_tray_icon = new QSystemTrayIcon{this};
    m_system_tray_icon->show();

    ui->setupUi(this);

    auto properties_combo = ui->properties_combo;
    properties_combo->setModel(m_property_model);
    properties_combo->setModelColumn(1);
    properties_combo->setPlaceholderText("Select a Property");
    properties_combo->setCurrentIndex(-1);

    auto units_list = ui->units_list_view;
    units_list->setViewMode(QListView::ListMode);
    units_list->setModel(m_unit_model);
    units_list->setModelColumn(1);
    units_list->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(properties_combo, &QComboBox::currentIndexChanged, this, &Manor::change_property);
    connect(ui->units_list_view, &QListView::clicked, this, &Manor::handle_unit_activated);

    connect(ui->action_Quit, &QAction::triggered, this, &Manor::quitApp);
    connect(ui->actionNew_Property, &QAction::triggered, this, &Manor::addProperty);
    connect(ui->action_new_unit, &QAction::triggered, this, &Manor::add_unit);
}

Manor::~Manor()
{
    delete ui;
}

void Manor::addProperty()
{
    PropertyDialog* dialog = new PropertyDialog(m_property_model, this);
    if (dialog->exec()) {
        qInfo() << "OK";
        m_system_tray_icon->showMessage("Property created", "Success!", QSystemTrayIcon::Information, 3000);
    } else {
        qInfo() << "Cancel";
    }
}

void Manor::change_property(int row)
{
    auto idx = m_property_model->index(row, 0);
    m_unit_model->setFilter("property_id = " + idx.data().toString());
    ui->selected_property_label->setText(idx.sibling(row, 1).data().toString());
    ui->selected_unit_label->setText(QString());
}

void Manor::quitApp()
{
    this->close();
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


void Manor::handle_unit_activated(const QModelIndex &idx)
{

    ui->selected_unit_label->setText(idx.data().toString());
}

