#ifndef SENDER_OPTIONS_H
#define SENDER_OPTIONS_H

//qt lib
#include <QDialog>
#include <QLineEdit>

//my headers
#include "ui_sender_options.h"

class sender_options : public QDialog
{
    Q_OBJECT

public:
    sender_options(QDialog *parent = nullptr);
    //UI components
    QLineEdit *lineEdit_ip;
    QLineEdit *lineEdit_port_number;
    QLineEdit *lineEdit_packet_size;
    QLineEdit *lineEdit_packet_count;
    QDialogButtonBox *btn_box;

private:
    Ui::sender_options ui;
};

#endif // SENDER_OPTIONS_H
