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
        ui->label_total_transfer_time->setText("Total Transfer Time: ");
        ui->label_total_data_transferred->setText("Total Data Transferred: ");
        ui->progress_bar->setValue(0);
        is_receiver = is_sender = is_tcp = is_udp = false;
        tcp_server = nullptr;
        tcp_socket = nullptr;
        udp_socket = nullptr;
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
        file_name = "receiver.txt";
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
    file_sender.open(file_name.toStdString());
    file_sender.seekg(0, file_sender.end);
    file_size = file_sender.tellg();
    file_sender.seekg(0, file_sender.beg);

    ui->btn_select_file->setEnabled(false);
    ui->btn_tcp->setEnabled(true);
    ui->btn_udp->setEnabled(true);
}

void MainWindow::onclick_btn_save_to_file()
{
    file_name = QFileDialog::getSaveFileName(this,
                                             tr(""),
                                             "",
                                             tr("Text File (*.txt)"));
    //    qDebug() << new QString(receiver_buffer);
    file_receiver.open(file_name.toStdString());
    file_receiver << receiver_buffer.toStdString();
    file_receiver.seekg(0, file_receiver.end);
    file_size = file_receiver.tellg();
    file_receiver.seekg(0, file_receiver.beg);
    file_receiver.close();
    file_receiver.open(file_name.toStdString());
}

void MainWindow::onclick_btn_start()
{
    ui->btn_stop->setEnabled(true);
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

    connect(this, SIGNAL(signal_starting_send()), this, SLOT(slot_start_timer()));
    emit signal_starting_send();

    double current = 0;
    double total = file_size * sender_packet_count;

    memset(sender_buffer, '\0', sender_packet_size + 1);

    while(!file_sender.eof()){

        file_sender.read(sender_buffer, sender_packet_size);

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
    connect(this, SIGNAL(signal_starting_send()), this, SLOT(slot_start_timer()));
    emit signal_starting_send();
    while(!file_sender.eof()){
        file_sender.read(sender_buffer, sender_packet_size);
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
    connect(tcp_server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if(!tcp_server->listen(QHostAddress::Any, receiver_port_number)) qDebug() << "Error: Listening to port";
    else qDebug() << "Success: Listening to port " << receiver_port_number;
}

void MainWindow::newConnection()
{
    tcp_socket = tcp_server->nextPendingConnection();

    connect(tcp_socket, &QTcpSocket::disconnected, this, &MainWindow::onclick_btn_stop);
    connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void MainWindow::receiver_udp()
{
    udp_socket = new QUdpSocket(this);
    udp_socket->bind(QHostAddress::Any, receiver_port_number);
    connect(this, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void MainWindow::readyRead()
{
    if(is_receiver && is_tcp){
        receiver_buffer.append(tcp_socket->readAll());
        qDebug() << new QString(receiver_buffer.constData());
        //        file_receiver << temp.toStdString();
    }

    if(is_receiver && is_udp){
        receiver_buffer.resize(udp_socket->pendingDatagramSize());
        int buffer_size = udp_socket->readDatagram(receiver_buffer.data(), receiver_buffer.size(), nullptr, nullptr);
        file_receiver << receiver_buffer.toStdString();
        if(buffer_size == 0) onclick_btn_stop();
    }
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
    connect(this, SIGNAL(signal_finished_send()), this, SLOT(slot_stop_timer()));
    emit signal_finished_send();

    update_transfer_statistics();

    file_sender.close();
    tcp_socket->disconnectFromHost();
}

void MainWindow::sender_udp_stop()
{
    connect(this, SIGNAL(signal_finished_send()), this, SLOT(slot_stop_timer()));
    emit signal_finished_send();
    update_transfer_statistics();
    file_sender.close();
    udp_socket->disconnectFromHost();
}

void MainWindow::receiver_tcp_stop()
{
    //    file_receiver.close();
    //    std::ifstream temp(file_name.toStdString().c_str(), std::fstream::ate | std::fstream::binary);
    //    file_size = temp.tellg();

    //    file_receiver.open(file_name.toStdString());
    //    file_receiver.seekg(0, file_receiver.end);
    //    file_size = file_receiver.tellg();
    //    file_receiver.seekg(0, file_receiver.beg);

    tcp_socket->disconnectFromHost();
    tcp_server->disconnect();
}

void MainWindow::receiver_udp_stop()
{
    file_receiver.close();
    std::ifstream temp(file_name.toStdString().c_str(), std::fstream::ate | std::fstream::binary);
    file_size = temp.tellg();
    udp_socket->disconnectFromHost();
}

void MainWindow::update_transfer_statistics()
{
    ui->label_total_data_transferred->setText(ui->label_total_data_transferred->text().append(QString::number(file_size * sender_packet_count)));
}

void MainWindow::slot_start_timer()
{
    timer.start();
}

void MainWindow::slot_stop_timer()
{
    total_transfer_time = timer.msec() / 1000.0;
    ui->label_total_transfer_time->setText(ui->label_total_transfer_time->text().append(QString::number(total_transfer_time)).append(" s"));
}
