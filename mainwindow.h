#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//qt lib
#include <QMainWindow>
#include <QFiledialog>

//std lib
#include <fstream>

//my headers
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
    //UI components
    Ui::MainWindow *ui;
    sender_options *s_options;
    receiver_options *r_options;

    //state variables
    bool is_receiver;
    bool is_sender;
    bool is_tcp;
    bool is_udp;

    //file variables
    QString file_name;
    int file_size;
    std::ifstream file_selected;
    std::ofstream file_saved;

    void init_ui();
    void onclick_btn_sender();
    void onclick_btn_receiver();
    void onclick_btn_tcp();
    void onclick_btn_udp();
    void onclick_btn_select_file();
    void onclick_btn_save_to_file();
    void onclick_btn_start();

};

#endif // MAINWINDOW_H
