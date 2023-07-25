#include "manor.h"
#include "./ui_manor.h"
#include <QListView>
#include <QtLogging>

Manor::Manor(const QString& propertyTable, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::Manor)
{

    m_PropertyModel = new QSqlTableModel(this);
    m_PropertyModel->setTable(propertyTable);
    // m_PropertyModel->setRelation(2, QSqlRelation(propertyTable, "id", "artist"));
    m_PropertyModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    // m_PropertyModel->relationModel(2)->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_PropertyModel->select();

    auto count = m_PropertyModel->rowCount();

    qInfo() << "count: " << count;

    ui->setupUi(this);

    auto propertiesList = ui->propertiesListView;
    propertiesList->setViewMode(QListView::ListMode);
    propertiesList->setModel(m_PropertyModel);
    propertiesList->setModelColumn(1);
    propertiesList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->action_Quit, &QAction::triggered, this, &Manor::quitApp);
}

Manor::~Manor()
{
    delete ui;
}

void Manor::quitApp()
{
    this->close();
}
