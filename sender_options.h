#ifndef SENDER_OPTIONS_H
#define SENDER_OPTIONS_H

#include <QDialog>
#include <QLineEdit>

#include "ui_sender_options.h"

class sender_options : public QDialog
{
    Q_OBJECT

public:
    explicit sender_options(QDialog *parent = nullptr);
    QLineEdit *label_ip;
    QLineEdit *label_port_number;
    QLineEdit *label_packet_size;
    QLineEdit *label_packet_count;

private:
    Ui::sender_options ui;
};

#endif // SENDER_OPTIONS_H
