#include "receiver_options.h"
/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: receiver_options.cpp - A Qt Dialog class for user to configure receiver settings.
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
--              This class initializes the user specified IP address and port number.
--              If not specified, it is set to default values.
----------------------------------------------------------------------------------------------------------------------*/
receiver_options::receiver_options(QDialog *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    lineEdit_ip = ui.lineEdit_ip;
    lineEdit_ip->setText("192.168.0.36");

    lineEdit_port_number = ui.lineEdit_port_number;
    lineEdit_port_number->setText("5151");
}
