#include "manor.h"
#include "./ui_manor.h"
#include "propertydialog.h"
#include <QListView>
#include <QtLogging>
#include <QSqlRelation>

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


    m_unit_model->select();

    qDebug() << "property count: " << m_property_model->rowCount()  << ", unit count: " << m_unit_model->rowCount();


    ui->setupUi(this);

    auto propertiesList = ui->propertiesListView;
    propertiesList->setViewMode(QListView::ListMode);
    propertiesList->setModel(m_property_model);
    propertiesList->setModelColumn(1);
    propertiesList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto units_list = ui->tab_1_list_view;
    units_list->setViewMode(QListView::ListMode);
    units_list->setModel(m_unit_model);
    units_list->setModelColumn(1);
    units_list->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->action_Quit, &QAction::triggered, this, &Manor::quitApp);
    connect(ui->actionNew_Property, &QAction::triggered, this, &Manor::addProperty);

    connect(ui->propertiesListView, &QListView::clicked, this, &Manor::change_property);
    connect(ui->propertiesListView, &QListView::activated, this, &Manor::change_property);
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
    } else {
        qInfo() << "Cancel";
    }
}

void Manor::change_property(QModelIndex idx)
{
    auto id_idx = m_property_model->index(idx.row(), 0);
    m_unit_model->setFilter("property_id = " + id_idx.data().toString());
}

void Manor::quitApp()
{
    this->close();
}
