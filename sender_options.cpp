#include "sender_options.h"
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: sender_options.cpp - A Qt Dialog class for user to configure sender settings.
--
-- PROGRAM:     tcpudp
--
-- FUNCTIONS:   N/A
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
--              This class initializes the user specified destination IP address, port number,
--              packet size and number of packets to send. If not specified, it is set to default
--              values.
----------------------------------------------------------------------------------------------------------------------*/
sender_options::sender_options(QDialog *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    lineEdit_ip = ui.lineEdit_ip;
    lineEdit_ip->setText("193.161.193.99");

    lineEdit_port_number = ui.lineEdit_port_number;
    lineEdit_port_number->setText("32907");

    lineEdit_packet_size = ui.lineEdit_packet_size;
    lineEdit_packet_size->setText("10");

    lineEdit_packet_count = ui.lineEdit_packet_count;
    lineEdit_packet_count->setText("10");
}
