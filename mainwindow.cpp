#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sender_options.h"

#include <QDebug>
#include <QNetworkDatagram>

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: mainwindow.cpp - A program that transfers/receives text files via TCP/UDP protocol.
--
-- PROGRAM:     tcpudp
--
-- FUNCTIONS:
--              void onclick_btn_sender();
--              void onclick_btn_receiver();
--              void onclick_btn_tcp();
--              void onclick_btn_udp();
--              void onclick_btn_select_file();
--              void onclick_btn_save_to_file();
--              void onclick_btn_start();
--              void onclick_btn_stop();

--              void sender_tcp();
--              void sender_udp();
--              void sender_tcp_stop();
--              void sender_udp_stop();

--              void receiver_tcp();
--              void receiver_udp();
--              void receiver_tcp_stop();
--              void receiver_udp_stop();

--              void init_ui();
--              void init_sender_variables();
--              void init_receiver_variables();
--              void update_transfer_statistics();
--              
--              // Slots
--              void newConnection();
--              void readyRead();
--              void slot_start_timer();
--              void slot_stop_timer();
--
-- DATE:        Feb.12, 2019
--
-- REVISIONS:   None
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- NOTES:
--              The program gives the user two choices: sender or receiver.
--                  Sender:
--                      Once the user selects the sender option, the user is able to:
--                          - specify destination IP address
--                          - specify destination port number
--                          - specify the packet size
--                          - specify the number of packet(s) to send
--                          - select a text file from their machine
--                          - select a protocol to use to send (TCP/UDP)
--                  Receiver:
--                      Once the user selects the receiver option, the user is able to:
--                          - specify destination IP address
--                          - specify destination port number
--                          - save the received text file to their machine
--                          - select a protocol to use to receive (TCP/UDP)
----------------------------------------------------------------------------------------------------------------------*/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow),
                                          s_options(new sender_options),
                                          r_options(new receiver_options)
{
    ui->setupUi(this);
    init_ui();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    init_ui
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void init_ui()
--
-- RETURNS:     void
--
-- NOTES:
--              This function connects all the UI components to it's UI event handlers and sets 
--              the initial states for all the UI components.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::init_ui()
{
    ui->progress_bar->hide();
    ui->receiver_console->hide();
    ui->label_preview->hide();

    connect(ui->btn_sender, &QPushButton::clicked, this, &MainWindow::onclick_btn_sender);
    connect(ui->btn_receiver, &QPushButton::clicked, this, &MainWindow::onclick_btn_receiver);

    connect(ui->btn_tcp, &QPushButton::clicked, this, &MainWindow::onclick_btn_tcp);
    connect(ui->btn_udp, &QPushButton::clicked, this, &MainWindow::onclick_btn_udp);

    connect(ui->btn_select_file, &QPushButton::clicked, this, &MainWindow::onclick_btn_select_file);
    connect(ui->btn_save_to_file, &QPushButton::clicked, this, &MainWindow::onclick_btn_save_to_file);

    connect(ui->btn_start, &QPushButton::clicked, this, &MainWindow::onclick_btn_start);
    connect(ui->btn_stop, &QPushButton::clicked, this, &MainWindow::onclick_btn_stop);

    connect(ui->menu_reset, &QAction::triggered, this, [&]() {
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    onclick_btn_sender
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void onclick_btn_sender()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is called when the user selects the "sender" button.
--              It triggers the sender options dialog and enables sender functionalities on exit.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::onclick_btn_sender()
{
    s_options->show();
    connect(s_options, &QDialog::accepted, this, [&]() {
        is_sender = true;
        ui->btn_sender->setEnabled(false);
        ui->btn_receiver->setEnabled(false);
        ui->btn_select_file->setEnabled(true);
        ui->progress_bar->show();
        init_sender_variables();
    });
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    init_sender_variables
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void init_sender_variables()
--
-- RETURNS:     void
--
-- NOTES:
--              This function initializes the user specified parameters for sender option: 
--                  IP address, port, packet size and number of packets
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::init_sender_variables()
{
    sender_ip = s_options->lineEdit_ip->text();
    sender_port_number = s_options->lineEdit_port_number->text().toInt();
    sender_packet_size = s_options->lineEdit_packet_size->text().toInt();
    sender_packet_count = s_options->lineEdit_packet_count->text().toInt();
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    onclick_btn_receiver
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void onclick_btn_receiver()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is called when the user selects the "receiver" button.
--              It triggers the receiver options dialog and enables receiver functionalities on 
--              exit.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::onclick_btn_receiver()
{
    r_options->show();
    connect(r_options, &QDialog::accepted, this, [&]() {
        is_receiver = true;
        ui->btn_tcp->setEnabled(true);
        ui->btn_udp->setEnabled(true);
        ui->btn_sender->setEnabled(false);
        ui->btn_receiver->setEnabled(false);
        ui->btn_save_to_file->setEnabled(true);
        ui->receiver_console->show();
        ui->label_preview->show();
        file_name = "receiver.txt";
        init_receiver_variables();
    });
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    init_receiver_variables
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void init_receiver_variables()
--
-- RETURNS:     void
--
-- NOTES:
--              This function initializes the user specified parameters for receiver option: 
--                  IP address and port 
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::init_receiver_variables()
{
    receiver_ip = r_options->lineEdit_ip->text();
    receiver_port_number = r_options->lineEdit_port_number->text().toInt();
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    onclick_btn_tcp
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void onclick_btn_tcp()
--
-- RETURNS:     void
--
-- NOTES:
--              This function flags that the user has chosen TCP as the protocol and enables next 
--              set of sender functionalities.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::onclick_btn_tcp()
{
    is_tcp = true;
    ui->btn_tcp->setEnabled(false);
    ui->btn_udp->setEnabled(false);
    ui->btn_start->setEnabled(true);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    onclick_btn_udp
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void onclick_btn_udp()
--
-- RETURNS:     void
--
-- NOTES:
--              This function flags that the user has chosen UDP as the protocol and enables next 
--              set of sender functionalities.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::onclick_btn_udp()
{
    is_udp = true;
    ui->btn_tcp->setEnabled(false);
    ui->btn_udp->setEnabled(false);
    ui->btn_start->setEnabled(true);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    onclick_btn_select_file
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void onclick_btn_select_file()
--
-- RETURNS:     void
--
-- NOTES:
--              This function opens the file explorer for the user to select the text file to be 
--              sent and enables next set of sender functionalities.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::onclick_btn_select_file()
{
    file_name = QFileDialog::getOpenFileName(this, tr("Select a .txt file to transfer"), "./", tr("Text File (*.txt)"));
    file_sender.open(file_name.toStdString());
    file_sender.seekg(0, file_sender.end);
    file_size = file_sender.tellg();
    file_sender.seekg(0, file_sender.beg);

    ui->btn_select_file->setEnabled(false);
    ui->btn_tcp->setEnabled(true);
    ui->btn_udp->setEnabled(true);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    onclick_btn_save_to_file
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void onclick_btn_save_to_file()
--
-- RETURNS:     void
--
-- NOTES:
--              This function opens the file explorer for the user to save the received text file.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::onclick_btn_save_to_file()
{
    file_name = QFileDialog::getSaveFileName(this, tr(""), "", tr("Text File (*.txt)"));
    file_receiver->rename(file_name);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    onclick_btn_start
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void onclick_btn_start()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is called when the user clicks the "start" button, having selected 
--              either sender/receiver and specified all the necessary options.
--              The function enables the stop button and determines which pair of options the user
--              had selected to start.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::onclick_btn_start()
{
    ui->btn_stop->setEnabled(true);

    if (is_sender && is_tcp)
    {
        sender_tcp();
    }
    else if (is_sender && is_udp)
    {
        sender_udp();
    }
    else if (is_receiver && is_tcp)
    {
        receiver_tcp();
    }
    else if (is_receiver && is_udp)
    {
        receiver_udp();
    }
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    sender_tcp
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void sender_tcp()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is called when the user has chosen the pair: sender + TCP + start.
--              It creates a TCP socket and connects/transfers data to the user defined host.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::sender_tcp()
{
    tcp_socket = new QTcpSocket(this);
    tcp_socket->connectToHost(QHostAddress(sender_ip), sender_port_number);

    connect(this, SIGNAL(signal_starting_send()), this, SLOT(slot_start_timer()));
    emit signal_starting_send();

    double current = 0;
    double total = file_size * sender_packet_count;

    memset(sender_buffer, '\0', sender_packet_size + 1);

    while (!file_sender.eof())
    {
        file_sender.read(sender_buffer, sender_packet_size);

        for (int i = 0; i < sender_packet_count; ++i)
        {
            int current_progress = (current / total) * 100;
            current += tcp_socket->write(sender_buffer);
            ui->progress_bar->setValue(current_progress);
        }
        memset(sender_buffer, '\0', sender_packet_size + 1);
    }

    file_sender.seekg(0, file_sender.beg);
    ui->progress_bar->setValue(100);
    onclick_btn_stop();
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    sender_udp
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void sender_udp()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is called when the user has chosen the pair: sender + UDP + start.
--              It creates a UDP socket and connects/transfers data to the user defined host.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::sender_udp()
{
    udp_socket = new QUdpSocket(this);

    connect(this, SIGNAL(signal_starting_send()), this, SLOT(slot_start_timer()));
    emit signal_starting_send();

    double current = 0;
    double total = file_size * sender_packet_count;

    memset(sender_buffer, '\0', sender_packet_size + 1);

    while (!file_sender.eof())
    {
        file_sender.read(sender_buffer, sender_packet_size);

        for (int i = 0; i < sender_packet_count; ++i)
        {
            int current_progress = (current / total) * 100;
            current += udp_socket->writeDatagram(sender_buffer, QHostAddress(sender_ip), sender_port_number);
            ui->progress_bar->setValue(current_progress);
        }
        memset(sender_buffer, '\0', sender_packet_size + 1);
    }

    file_sender.seekg(0, file_sender.beg);
    udp_socket->writeDatagram("", QHostAddress(sender_ip), sender_port_number);

    ui->progress_bar->setValue(100);
    onclick_btn_stop();
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    receiver_tcp
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void receiver_tcp()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is called when the user has chosen the pair: receiver + TCP + start.
--              It creates a TCP server and listens to the incoming data from the user defined host.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::receiver_tcp()
{
    tcp_server = new QTcpServer(this);
    file_receiver = new QFile("temp.txt");
    file_receiver->open(QIODevice::WriteOnly | QIODevice::Text);

    connect(tcp_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(this, SIGNAL(signal_starting_send()), this, SLOT(slot_start_timer()));
    emit signal_starting_send();

    if (!tcp_server->listen(QHostAddress::Any, receiver_port_number))
        qDebug() << "Error: Listening to port";
    else
        qDebug() << "Success (TCP): Listening to port " << receiver_port_number;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    newConnection
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void newConnection()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is a Qt slot that gets triggered when the TCP server gets an incoming
--              connection. The function binds the server to the socket.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::newConnection()
{
    tcp_socket = tcp_server->nextPendingConnection();

    connect(tcp_socket, &QTcpSocket::disconnected, this, &MainWindow::onclick_btn_stop);
    connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    receiver_udp
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void receiver_udp()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is called when the user has chosen the pair: receiver + UDP + start.
--              It creates a new UDP socket and binds to the user defined host.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::receiver_udp()
{
    udp_socket = new QUdpSocket(this);
    udp_socket->bind(QHostAddress::Any, receiver_port_number);
    file_receiver = new QFile("temp.txt");
    file_receiver->open(QIODevice::WriteOnly | QIODevice::Text);

    connect(this, SIGNAL(signal_starting_send()), this, SLOT(slot_start_timer()));
    emit signal_starting_send();

    connect(udp_socket, &QUdpSocket::readyRead, this, &MainWindow::readyRead);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    readyRead
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void readyRead()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is a Qt slot that gets triggered when either the TCP or UDP socket 
--              gets incoming data. It reads the data from the socket and write to the file and 
--              displays on the preview console.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::readyRead()
{
    QTextStream stream(file_receiver);

    if (is_receiver && is_tcp)
    {
        QByteArray data = tcp_socket->readAll();
        stream << data;
        ui->receiver_console->appendPlainText(data);
        onclick_btn_stop();
    }

    if (is_receiver && is_udp)
    {
        receiver_buffer.resize(udp_socket->pendingDatagramSize());
        udp_socket->readDatagram(receiver_buffer.data(), receiver_buffer.size(), nullptr, nullptr);
        stream << receiver_buffer;
        ui->receiver_console->appendPlainText(receiver_buffer);

        if (receiver_buffer.size() == 0)
            onclick_btn_stop();
    }

    file_size = file_receiver->size();
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    onclick_btn_stop
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void onclick_btn_stop()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is called when the user clicks the "stop" button.
--              The function determines which pair of options the user had selected to stop.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::onclick_btn_stop()
{
    if (is_sender && is_tcp)
    {
        sender_tcp_stop();
    }
    else if (is_sender && is_udp)
    {
        sender_udp_stop();
    }
    else if (is_receiver && is_tcp)
    {
        receiver_tcp_stop();
    }
    else if (is_receiver && is_udp)
    {
        receiver_udp_stop();
    }
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    sender_tcp_stop
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void sender_tcp_stop()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is called when the user has chosen the pair: sender + TCP + stop.
--              It cleans up all the aquired resources and updates the transfer statistics.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::sender_tcp_stop()
{
    connect(this, SIGNAL(signal_finished_send()), this, SLOT(slot_stop_timer()));
    emit signal_finished_send();
    update_transfer_statistics();
    file_sender.close();
    tcp_socket->disconnectFromHost();
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    sender_udp_stop
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void sender_udp_stop()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is called when the user has chosen the pair: sender + UDP + stop.
--              It cleans up all the aquired resources and updates the transfer statistics.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::sender_udp_stop()
{
    connect(this, SIGNAL(signal_finished_send()), this, SLOT(slot_stop_timer()));
    emit signal_finished_send();
    update_transfer_statistics();
    file_sender.close();
    udp_socket->disconnectFromHost();
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    receiver_tcp_stop
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void receiver_tcp_stop()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is called when the user has chosen the pair: receiver + TCP + stop.
--              It cleans up all the aquired resources and updates the transfer statistics.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::receiver_tcp_stop()
{
    connect(this, SIGNAL(signal_finished_send()), this, SLOT(slot_stop_timer()));
    emit signal_finished_send();
    update_transfer_statistics();
    file_receiver->close();
    tcp_socket->disconnectFromHost();
    tcp_server->disconnect();
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    receiver_udp_stop
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void receiver_udp_stop()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is called when the user has chosen the pair: receiver + UDP + stop.
--              It cleans up all the aquired resources and updates the transfer statistics.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::receiver_udp_stop()
{
    connect(this, SIGNAL(signal_finished_send()), this, SLOT(slot_stop_timer()));
    emit signal_finished_send();
    update_transfer_statistics();
    file_receiver->close();
    udp_socket->disconnectFromHost();
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    update_transfer_statistics
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void update_transfer_statistics()
--
-- RETURNS:     void
--
-- NOTES:
--              This function updates the UI responsible for displaying total data transferred.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::update_transfer_statistics()
{
    ui->label_total_data_transferred->setText(QString("Total Data Transferred: ").append(QString::number((file_size * sender_packet_count)/1000.0)).append(" KB"));
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    slot_start_timer
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void slot_start_timer()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is a Qt slot that gets triggered when an operation (send/receive) started
--              and starts the timer to measure the elapsed time.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::slot_start_timer()
{
    timer.start();
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:    slot_stop_timer
--
-- DATE:        Feb.12, 2019
--
-- DESIGNER:    Daniel Shin
--
-- PROGRAMMER:  Daniel Shin
--
-- INTERFACE:   void slot_stop_timer()
--
-- RETURNS:     void
--
-- NOTES:
--              This function is a Qt slot that gets triggered when an operation (send/receive) finished
--              and calculates the elapsed time in seconds, displaying it to the user.
----------------------------------------------------------------------------------------------------------------------*/
void MainWindow::slot_stop_timer()
{
    total_transfer_time = timer.msec() / 1000.0;
    ui->label_total_transfer_time->setText(QString("Total Transfer Time: ").append(QString::number(total_transfer_time)).append(" s"));
}
