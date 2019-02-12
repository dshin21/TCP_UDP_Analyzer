#include "sender_options.h"

sender_options::sender_options(QDialog *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    lineEdit_ip = ui.lineEdit_ip;
    lineEdit_ip->setText("localhost");

    lineEdit_port_number = ui.lineEdit_port_number;
    lineEdit_port_number->setText("8080");

    lineEdit_packet_size = ui.lineEdit_packet_size;
    lineEdit_packet_size->setText("10");

    lineEdit_packet_count = ui.lineEdit_packet_count;
    lineEdit_packet_count->setText("10");
}
