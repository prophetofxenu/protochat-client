#include "src/include/protochatmainwindow.h"
#include "ui_protochatmainwindow.h"
#include "src/include/protochatsocket.h"

#include <iostream>
#include <QTcpSocket>
#include <QTcpServer>
#include <chrono>
#include <thread>
#include <cstring>

ProtochatMainWindow::ProtochatMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProtochatMainWindow)
{
    ui->setupUi(this);

    string addr = "127.0.0.1";
    ProtochatSocket sock(addr, 20000);
    sock.connect();
}

ProtochatMainWindow::~ProtochatMainWindow()
{
    delete ui;
}
