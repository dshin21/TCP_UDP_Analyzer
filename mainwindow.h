#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "sender_options.h"
#include "receiver_options.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    sender_options *s_options;
    receiver_options *r_options;

    bool is_receiver;
    bool is_sender;
    bool is_tcp;
    bool is_udp;

    void init_ui();
    void onclick_btn_sender();
    void onclick_btn_receiver();
    void onclick_btn_tcp();
    void onclick_btn_udp();

};

#endif // MAINWINDOW_H
