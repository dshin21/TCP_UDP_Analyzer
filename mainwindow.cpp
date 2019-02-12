#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sender_options.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    s_options(new sender_options),
    r_options(new receiver_options)
{
    ui->setupUi(this);
    init_ui();
}

void MainWindow::init_ui()
{
    connect(ui->btn_sender, &QPushButton::clicked, this, &MainWindow::onclick_btn_sender);
    connect(ui->btn_receiver, &QPushButton::clicked, this, &MainWindow::onclick_btn_receiver);

    ui->btn_tcp->setEnabled(false);
    ui->btn_udp->setEnabled(false);
    ui->btn_start->setEnabled(false);
    ui->btn_stop->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onclick_btn_sender()
{
    s_options->show();
    connect(r_options, &QDialog::accepted, this, [&](){is_sender = true;});
}

void MainWindow::onclick_btn_receiver()
{
    r_options->show();
    connect(r_options, &QDialog::accepted, this, [&](){is_receiver = true;});
}

