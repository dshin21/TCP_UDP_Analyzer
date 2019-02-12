#ifndef RECEIVER_OPTIONS_H
#define RECEIVER_OPTIONS_H

//qt lib
#include <QDialog>
#include <QLineEdit>

//my headers
#include "ui_receiver_options.h"

class receiver_options : public QDialog
{
    Q_OBJECT

public:
    receiver_options(QDialog *parent = nullptr);

    //UI components
    QLineEdit *lineEdit_ip;
    QLineEdit *lineEdit_port_number;
    QDialogButtonBox *btn_box;

private:
    Ui::receiver_options ui;
};

#endif // RECEIVER_OPTIONS_H

