#include "receiver_options.h"
#include <QDebug>

receiver_options::receiver_options(QDialog *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
    lineEdit_ip = ui.lineEdit_ip;
    lineEdit_ip->setText("localhost");

    lineEdit_port_number = ui.lineEdit_port_number;
    lineEdit_port_number->setText("8080");
}
