#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sender_options.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    s_options(new sender_options),
    r_options(new receiver_options)
{
    ui->setupUi(this);
    init_ui();
}

void MainWindow::init_ui()
{
    connect(ui->btn_sender, &QPushButton::clicked, this, &MainWindow::onclick_btn_sender);
    connect(ui->btn_receiver, &QPushButton::clicked, this, &MainWindow::onclick_btn_receiver);

    connect(ui->btn_tcp, &QPushButton::clicked, this, &MainWindow::onclick_btn_tcp);
    connect(ui->btn_udp, &QPushButton::clicked, this, &MainWindow::onclick_btn_udp);

    connect(ui->btn_select_file, &QPushButton::clicked, this, &MainWindow::onclick_btn_select_file);
    connect(ui->btn_save_to_file, &QPushButton::clicked, this, &MainWindow::onclick_btn_save_to_file);

    connect(ui->menu_reset, &QAction::triggered, this, [&](){
        ui->btn_sender->setEnabled(true);
        ui->btn_receiver->setEnabled(true);
        ui->btn_tcp->setEnabled(false);
        ui->btn_udp->setEnabled(false);
        ui->btn_start->setEnabled(false);
        ui->btn_stop->setEnabled(false);
        ui->btn_save_to_file->setEnabled(false);
        ui->btn_select_file->setEnabled(false);
        is_receiver = is_sender = is_tcp = is_udp = false;
        s_options = new sender_options;
        r_options = new receiver_options;
    });

    ui->btn_tcp->setEnabled(false);
    ui->btn_udp->setEnabled(false);
    ui->btn_start->setEnabled(false);
    ui->btn_stop->setEnabled(false);
    ui->btn_select_file->setEnabled(false);
    ui->btn_save_to_file->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onclick_btn_sender()
{
    s_options->show();
    connect(s_options, &QDialog::accepted, this, [&](){
        is_sender = true;
        ui->btn_sender->setEnabled(false);
        ui->btn_receiver->setEnabled(false);
        ui->btn_select_file->setEnabled(true);
        init_sender_variables();
    });
}

void MainWindow::init_sender_variables()
{
    sender_ip = s_options->lineEdit_ip->text();
    sender_port_number = s_options->lineEdit_port_number->text().toInt();
    sender_packet_size = s_options->lineEdit_packet_size->text().toInt();
    sender_packet_count = s_options->lineEdit_packet_count->text().toInt();
    qDebug() <<  sender_ip << " " << sender_port_number << " " << sender_packet_size << " " << sender_packet_count;
}

void MainWindow::onclick_btn_receiver()
{
    r_options->show();
    connect(r_options, &QDialog::accepted, this, [&](){
        is_receiver = true;
        ui->btn_tcp->setEnabled(true);
        ui->btn_udp->setEnabled(true);
        ui->btn_sender->setEnabled(false);
        ui->btn_receiver->setEnabled(false);
        ui->btn_save_to_file->setEnabled(true);
        init_receiver_variables();
    });
}

void MainWindow::init_receiver_variables()
{
    receiver_ip = r_options->lineEdit_ip->text();
    receiver_port_number = r_options->lineEdit_port_number->text().toInt();
    qDebug() <<  receiver_ip << " " << receiver_port_number;
}

void MainWindow::onclick_btn_tcp()
{
    is_tcp = true;
    ui->btn_tcp->setEnabled(false);
    ui->btn_udp->setEnabled(false);
    ui->btn_start->setEnabled(true);
}

void MainWindow::onclick_btn_udp()
{
    is_udp = true;
    ui->btn_tcp->setEnabled(false);
    ui->btn_udp->setEnabled(false);
    ui->btn_start->setEnabled(true);
}

void MainWindow::onclick_btn_select_file()
{
    file_name = QFileDialog::getOpenFileName(this,
                                             tr("Select a .txt file to transfer"),
                                             "./",
                                             tr("Text File (*.txt)"));
    file_selected.open(file_name.toStdString().c_str(), std::fstream::ate | std::fstream::binary);
    file_size = file_selected.tellg();
    file_selected.seekg(0, file_selected.beg);

    ui->btn_select_file->setEnabled(false);
    ui->btn_tcp->setEnabled(true);
    ui->btn_udp->setEnabled(true);
}

void MainWindow::onclick_btn_save_to_file()
{
    file_name = QFileDialog::getSaveFileName(this,
                                             tr("results.txt"),
                                             "",
                                             tr("Text File (*.txt)"));
    file_saved.open(file_name.toStdString());
}

void MainWindow::onclick_btn_start()
{
    if(is_sender && is_tcp){
        sender_tcp();
    }else if(is_sender && is_udp){
        sender_udp();
    }else if(is_receiver && is_tcp){
        receiver_tcp();
    }else if(is_receiver && is_udp){
        receiver_udp();
    }
}

void MainWindow::sender_tcp()
{

}

void MainWindow::sender_udp()
{

}

void MainWindow::receiver_tcp()
{

}

void MainWindow::receiver_udp()
{

}

