#ifndef RECEIVER_OPTIONS_H
#define RECEIVER_OPTIONS_H

#include <QDialog>
#include <QLineEdit>

#include "ui_receiver_options.h"

class receiver_options : public QDialog
{
    Q_OBJECT

public:
    explicit receiver_options(QDialog *parent = nullptr);
    QLineEdit *label_ip;
    QLineEdit *label_port_number;

private:
    Ui::receiver_options ui;
};

#endif // RECEIVER_OPTIONS_H

