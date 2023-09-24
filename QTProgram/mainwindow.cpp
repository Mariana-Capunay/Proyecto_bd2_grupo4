#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QStandardItemModel>
#include <iostream>
#include <vector>
#include <string>
#include "../parser.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    this->table_set = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_exec_button_clicked(){
    // Obtiene el contenido del objeto
    QString text = ui->input_sql->toPlainText();
    std::string read_inp = text.toStdString();
    run_sql(read_inp);
    // create table tabla from file "dataset/10k/dataset_1.csv"
    // cmake-build-debug/dataset_1.csv
    clearTable();
    // Reemplazar condicionales por flags
    // Flag 3: Muestra el dataset completo (NtH)
    /*if(read_inp == "createTable"){                                      // Flag 1: Se ejecuta create exitosamente
        wipeOutTable();
        std::vector<std::vector<int>> test = {{1, 2, 3, 4, 5, 6}, {2, 4, 6, 8, 10, 12}, {3, 6, 9, 12, 15, 18}};
        ui->txt_output->setPlainText("Configurando tabla");
        setupTable(test);
        table_set = true;
    } else if(read_inp == "createTableF"){                              // Flag 2: Se ejecuta create pero no existe archivo
        ui->txt_output->setPlainText("Archivo no encontrado");
    } else if(read_inp == "setTableSearch1" && table_set){              // Flag 4: Búsquedas
        std::vector<std::vector<int>> test1 = {{1, 2, 3, 4, 5, 6}, {2, 4, 6, 8, 10, 12}, {3, 6, 9, 12, 15, 18}};
        ui->txt_output->setPlainText("Mostrando elementos");
        displayTable(test1);
    } else if(read_inp == "setTableSearch2" && table_set){
        std::vector<std::vector<int>> test2 = {{4, 8, 12, 16, 20, 24}, {5, 10, 15, 20, 25, 30}, {6, 12, 18, 24, 30, 36}};
        ui->txt_output->setPlainText("Mostrando elementos");
        displayTable(test2);
    }else if(read_inp == "insertIntoTable"){                            // Flag 5: Inserción exitosa
        ui->txt_output->setPlainText("Insertando elemento en tabla");
    } else if(read_inp == "removeFromTable"){                           // Flag 6: Remove exitoso
        ui->txt_output->setPlainText("Eliminando elemento en tabla");
    } else{
        ui->txt_output->setPlainText("ERROR: No se ha reconocido una funcion valida");
    }*/

    //std::cout << text.toStdString();


    //std::cout << res << std::endl;
    //QString output = QString::fromStdString(res);
    //ui->dsp_output->setPlainText(output);

}

void MainWindow::setupTable(std::vector<std::vector<int>> content){
    QString s_col1 = "Col1";
    QString s_col2 = "Col2";
    QString s_col3 = "Col3";
    QString s_col4 = "Col4";
    QString s_col5 = "Col5";
    QString s_col6 = "Col6";
    this->horizontalHeader.append(s_col1);
    this->horizontalHeader.append(s_col2);
    this->horizontalHeader.append(s_col3);
    this->horizontalHeader.append(s_col4);
    this->horizontalHeader.append(s_col5);
    this->horizontalHeader.append(s_col6);

    // Creación del modelo
    model = new QStandardItemModel();

    model->setHorizontalHeaderLabels(horizontalHeader);
    model->setVerticalHeaderLabels(verticalHeader);

    ui->tableView->setModel(model);

    ui->tableView->verticalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->verticalHeader()->setDefaultSectionSize(10);
    ui->tableView->setShowGrid(true);
}

void MainWindow::displayTable(std::vector<std::vector<int>> content){
    int cnt = 0;
    model->setHorizontalHeaderLabels(horizontalHeader);
    model->setVerticalHeaderLabels(verticalHeader);
    for(auto item: content){
        std::cout << "Insertando elemento " << ++cnt << std::endl;
        model->appendRow( QList<QStandardItem*>() <<
                                                  new QStandardItem(QString::fromStdString(std::to_string(item[0]))) <<
                                                  new QStandardItem(QString::fromStdString(std::to_string(item[1]))) <<
                                                  new QStandardItem(QString::fromStdString(std::to_string(item[2]))) <<
                                                  new QStandardItem(QString::fromStdString(std::to_string(item[3]))) <<
                                                  new QStandardItem(QString::fromStdString(std::to_string(item[4]))) <<
                                                  new QStandardItem(QString::fromStdString(std::to_string(item[5])))
        );
    }
    /*
    // Definición de atributos
    QStandardItem *col1 = new QStandardItem();
    QStandardItem *col2 = new QStandardItem();
    col1->setText("Atr1");
    col2->setText("Atr2");

    // Ingreso de elementos a la tabla
    model->appendRow( QList<QStandardItem*>() << new QStandardItem() << col2);
    col1->setText("Atr3");
    col2->setText("Atr4");
    model->appendRow( QList<QStandardItem*>() << col1 << col2);
*/
}

void MainWindow::clearTable(){
    model = new QStandardItemModel();
    ui->tableView->setModel(model);
    //QStandardItem *col1 = new QStandardItem("");    // Ingreso de elementos a la tabla
}

void MainWindow::wipeOutTable(){
    model = new QStandardItemModel();
    this->horizontalHeader.clear();
    this->verticalHeader.clear();
    ui->tableView->setModel(model);
    //QStandardItem *col1 = new QStandardItem("");    // Ingreso de elementos a la tabla
}
