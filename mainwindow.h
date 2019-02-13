#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//qt lib
#include <QMainWindow>
#include <QFiledialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QElapsedTimer>
#include <QTime>
#include <QFile>
#include <QTextStream>

//std lib
#include <fstream>

//my headers
#include "sender_options.h"
#include "receiver_options.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    //UI components
    Ui::MainWindow *ui;
    sender_options *s_options;
    receiver_options *r_options;

    //state variables
    bool is_receiver;
    bool is_sender;
    bool is_tcp;
    bool is_udp;

    //variables (sender)
    QString sender_ip;
    int sender_port_number;
    int sender_packet_size;
    int sender_packet_count;
    int total_bytes_transferred;

    char sender_buffer[65536];

    //variables (receiver)
    QString receiver_ip;
    int receiver_port_number;

    QByteArray receiver_buffer;

    //variables (tcp/udp)
    QTcpServer *tcp_server;
    QTcpSocket *tcp_socket;
    QUdpSocket *udp_socket;

    //file variables
    QString file_name;
    int file_size;
    std::fstream file_sender;
    QFile *file_receiver;

    //time variables
    double total_transfer_time;
    QTime timer;

    //functions (event handlers)
    void onclick_btn_sender();
    void onclick_btn_receiver();
    void onclick_btn_tcp();
    void onclick_btn_udp();
    void onclick_btn_select_file();
    void onclick_btn_save_to_file();
    void onclick_btn_start();
    void onclick_btn_stop();

    //functions (sender)
    void sender_tcp();
    void sender_udp();
    void sender_tcp_stop();
    void sender_udp_stop();

    //functions (receiver)
    void receiver_tcp();
    void receiver_udp();
    void receiver_tcp_stop();
    void receiver_udp_stop();

    //functions
    void init_ui();
    void init_sender_variables();
    void init_receiver_variables();
    void update_transfer_statistics();

public slots:
    void newConnection();
    void readyRead();
    void slot_start_timer();
    void slot_stop_timer();

signals:
    void signal_starting_send();
    void signal_finished_send();
    void signal_starting_read();
    void signal_finished_read();

};

#endif // MAINWINDOW_H
