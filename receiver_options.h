#ifndef RECEIVER_OPTIONS_H
#define RECEIVER_OPTIONS_H

#include <QDialog>
#include <QLineEdit>

#include "ui_receiver_options.h"

class receiver_options : public QDialog
{
    Q_OBJECT

public:
    receiver_options(QDialog *parent = nullptr);
    QLineEdit *lineEdit_ip;
    QLineEdit *lineEdit_port_number;
    QDialogButtonBox *btn_box;

private:
    Ui::receiver_options ui;

};

#endif // RECEIVER_OPTIONS_H

