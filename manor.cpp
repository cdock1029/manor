#include "manor.h"
#include "./ui_manor.h"
#include "database.h"
#include "leasewizard.h"
#include "qnamespace.h"
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

using namespace Qt::StringLiterals;

Manor::Manor(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::Manor)
    , m_tenant_model { new QSqlTableModel { this } }
{

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
    auto* const page_combo = new QComboBox { this };
    page_combo->addItem(u"Properties"_s);
    page_combo->addItem(u"Tenants"_s);

    ui->stackedWidget->setCurrentIndex(0);
    page_combo->setCurrentIndex(0);

    auto* const spacer = new QWidget { this };
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
    for (int column = 0; column < col_count; ++column) {
        if (!table.isColumnHidden(column)) {
            out << QString("<th>%1</th>").arg(table.model()->headerData(column, Qt::Horizontal).toString());
        }
    }
    out << "</tr></thead>\n";
    for (int row = 0; row < row_count; ++row) {
        out << "<tr>";
        for (int column = 0; column < col_count; ++column) {
            if (!table.isColumnHidden(column)) {
                const QString data = table.model()->data(table.model()->index(row, column)).toString().simplified();
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

void Manor::setup_property_tabs()
{
    QSqlQuery query { u"SELECT * FROM properties"_s };
    auto properties_model = QSqlQueryModel {};
    properties_model.setQuery(std::move(query));

    for (auto i = 0; i < properties_model.rowCount(); ++i) {
        const auto property_name = properties_model.record(i).field("name").value().toString();
        const auto property_id = properties_model.record(i).field("id").value().toInt();

        auto* page = new QWidget { ui->tabWidget };
        auto* layout = new QVBoxLayout { page };
        auto* table = new QTableView { page };

        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->setVisible(false);
        table->setContextMenuPolicy(Qt::CustomContextMenu);
        table->setSelectionBehavior(QAbstractItemView::SelectRows);
        table->setSortingEnabled(true);

        auto* const query_model = new QSqlQueryModel { table };
        query_model->setQuery(QSqlQuery { u"SELECT * FROM active_leases WHERE property_id = %1"_s.arg(property_id) });
        auto* const proxy_model = new QSortFilterProxyModel { this };
        proxy_model->setSourceModel(query_model);

        table->setModel(proxy_model);
        table->hideColumn(0);
        table->hideColumn(Db::ACTIVE_LEASES_LEASE_ID);
        table->hideColumn(Db::ACTIVE_LEASES_PROPERTY_ID);

        // override default sort column 0,db already sorts
        table->sortByColumn(-1, Qt::AscendingOrder);

        connect(table,
            &QTableView::customContextMenuRequested,
            this,
            [table, this](QPoint pos) {
                auto menu = QMenu {};
                auto action = QAction { u"Print table"_s };
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

        ui->tabWidget->addTab(page, property_name.toUpper());
    }
}

void Manor::setup_tenants_table()
{
    for (int i = 0; i < m_tenant_model->columnCount(); ++i) {
        m_tenant_model->setHeaderData(
            i, Qt::Horizontal, m_tenant_model->headerData(i, Qt::Horizontal).toString().toUpper());
    }
    auto* const tenants_table = ui->tenants_table_view;
    tenants_table->setModel(m_tenant_model);
    tenants_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tenants_table->hideColumn(0);
    tenants_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tenants_table->setSelectionBehavior(QAbstractItemView::SelectRows);

    tenants_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tenants_table,
        &QTableView::customContextMenuRequested,
        this,
        [=, this](QPoint pos) {
            auto menu = QMenu { this };
            menu.addAction(ui->action_delete_tenant);

            connect(ui->action_delete_tenant, &QAction::triggered, this, [=]() {
                QModelIndexList selected_idxs = tenants_table->selectionModel()->selectedIndexes();
                if (!selected_idxs.isEmpty()) {
                    auto row = selected_idxs.first().row();
                    auto* const model = qobject_cast<QSqlTableModel*>(tenants_table->model());
                    if (model != nullptr) {
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
    connect(ui->action_quit, &QAction::triggered, this, [this]() { this->close(); });
    connect(ui->action_new_tenant, &QAction::triggered, this, &Manor::add_tenant);
    connect(ui->action_New_Lease, &QAction::triggered, this, [this]() {
        auto* const wizard = new LeaseWizard(this);
        wizard->open();
    });
}

void Manor::add_tenant()
{
    auto dialog = QPointer { new TenantDialog { m_tenant_model, this } };
    dialog->open();
}
