#ifndef SENDER_OPTIONS_H
#define SENDER_OPTIONS_H

#include <QDialog>
#include <QLineEdit>

#include "ui_sender_options.h"

class sender_options : public QDialog
{
    Q_OBJECT

public:
    sender_options(QDialog *parent = nullptr);
    QLineEdit *lineEdit_ip;
    QLineEdit *lineEdit_port_number;
    QLineEdit *lineEdit_packet_size;
    QLineEdit *lineEdit_packet_count;
    QDialogButtonBox *btn_box;

private:
    Ui::sender_options ui;
};

#endif // SENDER_OPTIONS_H
