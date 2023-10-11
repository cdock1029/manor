#include "manor.h"
#include "./ui_manor.h"
#include "database.h"
#include "leasewizard.h"
#include "propertydialog.h"
#include "tenantdialog.h"
#include <QDoubleSpinBox>
#include <QInputDialog>
#include <QItemSelection>
#include <QMessageBox>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>
#include <QSortFilterProxyModel>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlRelationalDelegate>
#include <QStackedWidget>
#include <QWizard>
#include <utility>

Manor::Manor(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::Manor)
    , m_property_model { new QSqlTableModel { this } }
    , m_tenant_model { new QSqlTableModel { this } }
    , m_active_leases_model { new QSqlQueryModel { this } }
{
    m_property_model->setTable("properties");
    m_property_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_property_model->select();

    m_tenant_model->setTable("tenants");
    m_tenant_model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_tenant_model->setSort(3, Qt::AscendingOrder);

    ui->setupUi(this);

    setup_stack();

    setup_property_tabs();
    setup_tenants_table();
    setup_actions();
}

Manor::~Manor()
{
    delete ui;
}

void Manor::setup_stack()
{
    const auto page_combo = new QComboBox { this };
    page_combo->addItem(u"Properties"_qs);
    page_combo->addItem(u"Tenants"_qs);

    ui->stackedWidget->setCurrentIndex(0);
    page_combo->setCurrentIndex(0);

    const auto spacer = new QWidget { this };
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(spacer);
    ui->toolBar->addWidget(page_combo);

    connect(page_combo, &QComboBox::activated, ui->stackedWidget, &QStackedWidget::setCurrentIndex);
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [this](int index) {
        switch (index) {
        case 0:
            break;
        case 1:
            m_tenant_model->select();
            break;
        default:
            qWarning() << "unknown stack index: " << index;
            break;
        }
    });
}

auto table_to_string(const QTableView& table) -> QString
{
    const auto row_count = table.model()->rowCount();
    const auto col_count = table.model()->columnCount();
    auto str = QString {};
    auto out = QTextStream { &str };

    out << "<html>\n"
           "<head>\n"
           "<meta Content=\"text/html; charset=Windows-1251\">\n"
        << "<title>Table</title>\n"
        << "</head>\n"
           "<body bgcolor=#ffffff link=#5000A0>\n"
           "<table border=1 cellspacing=0 cellpadding=2>\n";

    out << "<thead><tr bgcolor=#f0f0f0>";
    for (int column = 0; column < col_count; ++column)
        if (!table.isColumnHidden(column))
            out << QString("<th>%1</th>").arg(table.model()->headerData(column, Qt::Horizontal).toString());
    out << "</tr></thead>\n";
    for (int row = 0; row < row_count; ++row) {
        out << "<tr>";
        for (int column = 0; column < col_count; ++column) {
            if (!table.isColumnHidden(column)) {
                QString data = table.model()->data(table.model()->index(row, column)).toString().simplified();
                out << QString("<td bkcolor=0>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
            }
        }
        out << "</tr>\n";
    }
    out << "</table>\n"
           "</body>\n"
           "</html>\n";
    return str;
}

void Manor::setup_property_tabs(int active_tab)
{
    ui->tabWidget->disconnect();
    while (ui->tabWidget->count()) {
        delete ui->tabWidget->widget(0);
    }
    ui->tabWidget->clear();

    for (auto i = 0; i < m_property_model->rowCount(); ++i) {
        auto layout = new QVBoxLayout {};
        auto page = new QWidget {};
        auto table = new QTableView {};

        const auto proxy_model = new QSortFilterProxyModel { this };
        proxy_model->setSourceModel(m_active_leases_model);
        table->setModel(proxy_model);

        table->setSortingEnabled(true);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->setVisible(false);
        table->setContextMenuPolicy(Qt::CustomContextMenu);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);

        connect(table, &QTableView::customContextMenuRequested, this, [table, this](QPoint pos) {
            auto menu = QMenu {};
            auto action = QAction { u"Print table"_qs };
            menu.addAction(&action);
            connect(&action, &QAction::triggered, this, [table, this]() {
                auto table_string = table_to_string(*table);
                auto doc = QTextDocument {};
                doc.setHtml(table_string);

                auto printer = QPrinter { QPrinter::ScreenResolution };
                printer.setOutputFormat(QPrinter::PdfFormat);
                auto print_dialog = QPrintDialog { &printer, this };
                if (print_dialog.exec() == QDialog::Accepted) {
                    doc.print(&printer);
                }
            });
            menu.exec(table->viewport()->mapToGlobal(pos));
        });
        layout->addWidget(table);
        page->setLayout(layout);

        const auto record = m_property_model->record(i);
        const auto name = record.field(u"name"_qs).value().toString();
        ui->tabWidget->addTab(page, name.toUpper());
    }
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        const auto property_id = m_property_model->index(index, 0).data().toInt();
        QSqlQuery query;
        query.prepare(u"SELECT * FROM active_leases WHERE property_id = :property_id"_qs);
        query.bindValue(u":property_id"_qs, property_id);
        query.exec();
        m_active_leases_model->setQuery(std::move(query));

        const auto tab_widget = qobject_cast<QTabWidget*>(sender());
        const auto page = tab_widget->currentWidget();
        const auto layout = qobject_cast<QVBoxLayout*>(page->layout());
        const auto table = qobject_cast<QTableView*>(layout->itemAt(0)->widget());
        const auto model = qobject_cast<QSortFilterProxyModel*>(table->model());
        // override default sort column 0, just display data as queried (view already orders by unit)
        model->sort(-1);

        table->hideColumn(0);
        table->hideColumn(Db::ACTIVE_LEASES_LEASE_ID);
        table->hideColumn(Db::ACTIVE_LEASES_PROPERTY_ID);
    });
    if (ui->tabWidget->currentIndex() != active_tab) {
        ui->tabWidget->setCurrentIndex(active_tab);
    } else {
        emit ui->tabWidget->currentChanged(active_tab);
    }
}

void Manor::setup_tenants_table()
{
    for (int i = 0; i < m_tenant_model->columnCount(); ++i) {
        m_tenant_model->setHeaderData(i, Qt::Horizontal, m_tenant_model->headerData(i, Qt::Horizontal).toString().toUpper());
    }
    const auto tenants_table = ui->tenants_table_view;
    tenants_table->setModel(m_tenant_model);
    tenants_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tenants_table->hideColumn(0);
    tenants_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tenants_table->setSelectionBehavior(QAbstractItemView::SelectRows);

    tenants_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tenants_table, &QTableView::customContextMenuRequested, this, [=, this](QPoint pos) {
        auto menu = QMenu { this };
        menu.addAction(ui->action_delete_tenant);

        connect(ui->action_delete_tenant, &QAction::triggered, this, [=]() {
            QModelIndexList selected_idxs = tenants_table->selectionModel()->selectedIndexes();
            if (!selected_idxs.isEmpty()) {
                auto row = selected_idxs.first().row();
                auto model = qobject_cast<QSqlTableModel*>(tenants_table->model());
                if (model) {
                    model->removeRow(row);
                    model->submitAll();
                }
            }
        });

        menu.exec(tenants_table->viewport()->mapToGlobal(pos));
    });
}

void Manor::setup_actions()
{
    connect(ui->action_quit, &QAction::triggered, this, [this]() {
        this->close();
    });
    connect(ui->action_new_property, &QAction::triggered, this, &Manor::add_property);
    // connect(ui->action_new_unit, &QAction::triggered, this, &Manor::add_unit);
    connect(ui->action_new_tenant, &QAction::triggered, this, &Manor::add_tenant);

    connect(ui->action_delete_property, &QAction::triggered, this, [=, this]() {
        const auto current = ui->tabWidget->currentIndex();
        if (current == -1) {
            QMessageBox::information(this, "Delete Property", "Select the Property you want to delete");
        } else {
            const auto idx = m_property_model->index(current, 0);
            const auto name = idx.sibling(idx.row(), 1).data().toString();
            const auto button = QMessageBox::question(this, "Delete Property", QString("Are you sure you want to"
                                                                                       "delete '%1' ?")
                                                                                   .arg(name),
                QMessageBox::Yes | QMessageBox::No);
            if (button == QMessageBox::Yes) {
                m_property_model->removeRow(idx.row());
                m_property_model->submitAll();
                setup_property_tabs();
            }
        }
    });

    connect(ui->action_New_Lease, &QAction::triggered, this, [this]() {
        auto wizard = QPointer { new LeaseWizard(this) };
        wizard->open();
    });
}

void Manor::add_property()
{
    auto dialog = QPointer { new PropertyDialog { m_property_model, this } };
    connect(dialog, &QDialog::accepted, this, [this]() {
        setup_property_tabs(ui->tabWidget->count());
    });
    dialog->open();
}

void Manor::add_tenant()
{
    auto dialog = QPointer { new TenantDialog { m_tenant_model, this } };
    dialog->open();
}

/*
void Manor::add_unit()
{
    auto combo_index = ui->tabWidget->currentIndex();
    // check if property selected
    if (combo_index >= 0) {
        // dialog for text input
        auto d = QInputDialog(this);
        d.setInputMode(QInputDialog::TextInput);
        d.setWindowTitle("New unit");
        d.setLabelText("Unit name");
        d.setOkButtonText("Save");
        const int width = 400;
        const int height = 200;
        d.resize(width, height);
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
*/
