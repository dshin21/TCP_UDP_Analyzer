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

    connect(ui->btn_start, &QPushButton::clicked, this, &MainWindow::onclick_btn_start);
    connect(ui->btn_stop, &QPushButton::clicked, this, &MainWindow::onclick_btn_stop);

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
    std::ifstream temp(file_name.toStdString().c_str(), std::fstream::ate | std::fstream::binary);
    file_size = temp.tellg();

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
    tcp_socket = new QTcpSocket(this);
    tcp_socket->connectToHost(QHostAddress(sender_ip), sender_port_number);

    double current = 0;
    double total = file_size * sender_packet_count;

    memset(sender_buffer, '\0', sender_packet_size + 1);
    while(!file_selected.eof()){
        file_selected.read(sender_buffer, sender_packet_size);
        for(int i = 0; i < sender_packet_count; ++i){
            int current_progress = (current / total) * 100;
            current += tcp_socket->write(sender_buffer);
            ui->progress_bar->setValue(current_progress);
        }
        memset(sender_buffer, '\0', sender_packet_size + 1);
    }

    ui->progress_bar->setValue(100);
    onclick_btn_stop();
}

void MainWindow::sender_udp()
{
    udp_socket = new QUdpSocket(this);
    double current = 0;
    double total = file_size * sender_packet_count;

    memset(sender_buffer, '\0', sender_packet_size + 1);
    while(!file_selected.eof()){
        file_selected.read(sender_buffer, sender_packet_size);
        for(int i = 0; i < sender_packet_count; ++i){
            int current_progress = (current / total) * 100;
            current += udp_socket->writeDatagram(sender_buffer, QHostAddress(sender_ip), sender_port_number);
            ui->progress_bar->setValue(current_progress);
        }
        memset(sender_buffer, '\0', sender_packet_size + 1);
    }

    ui->progress_bar->setValue(100);
    onclick_btn_stop();
}

void MainWindow::receiver_tcp()
{
    tcp_server = new QTcpServer(this);
    connect(tcp_server, SIGNAL(tcp_bind()), this, SLOT(tcp_bind()));
    if(tcp_server->listen(QHostAddress::Any, receiver_port_number)) qDebug() << "Error: Listening to port";
    else qDebug() << "Success: Listening to port " << receiver_port_number;
}

void MainWindow::tcp_bind()
{
    tcp_socket = tcp_server->nextPendingConnection();
    connect(tcp_socket, &QTcpSocket::disconnected, this, &MainWindow::onclick_btn_stop);
    connect(tcp_server, SIGNAL(tcp_read_data()), this, SLOT(tcp_read_data()));
}

void MainWindow::tcp_read_data()
{
    file_saved << tcp_socket->readAll().toStdString();
}

void MainWindow::receiver_udp()
{
    udp_socket = new QUdpSocket(this);
    udp_socket->bind(QHostAddress::Any, receiver_port_number);
    connect(udp_socket, SIGNAL(udp_read_data()), this, SLOT(udp_read_data()));
}

void MainWindow::udp_read_data()
{
    receiver_udp_buffer.resize(udp_socket->pendingDatagramSize());
    int buffer_size = udp_socket->readDatagram(receiver_udp_buffer.data(), receiver_udp_buffer.size(), nullptr, nullptr);
    file_saved << receiver_udp_buffer.toStdString();
    if(buffer_size == 0) onclick_btn_stop();
}

void MainWindow::onclick_btn_stop()
{
    if(is_sender && is_tcp){
        sender_tcp_stop();
    }else if(is_sender && is_udp){
        sender_udp_stop();
    }else if(is_receiver && is_tcp){
        receiver_tcp_stop();
    }else if(is_receiver && is_udp){
        receiver_udp_stop();
    }
}

void MainWindow::sender_tcp_stop()
{
    file_selected.close();
    tcp_socket->disconnectFromHost();
}

void MainWindow::sender_udp_stop()
{
    file_selected.close();
    udp_socket->disconnectFromHost();
}

void MainWindow::receiver_tcp_stop()
{
    file_saved.close();
    std::ifstream temp(file_name.toStdString().c_str(), std::fstream::ate | std::fstream::binary);
    file_size = temp.tellg();
    tcp_socket->disconnectFromHost();
    tcp_server->disconnect();
}

void MainWindow::receiver_udp_stop()
{
    file_saved.close();
    std::ifstream temp(file_name.toStdString().c_str(), std::fstream::ate | std::fstream::binary);
    file_size = temp.tellg();
    udp_socket->disconnectFromHost();
}
