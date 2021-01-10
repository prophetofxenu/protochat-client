#include "src/include/protochatmainwindow.h"
#include "ui_protochatmainwindow.h"
#include "src/include/protochatsocket.h"

#include <iostream>
#include <QTcpSocket>
#include <QTcpServer>
#include <chrono>
#include <thread>
#include <cstring>

#include <cryptopp/secblock.h>
using CryptoPP::byte;

ProtochatMainWindow::ProtochatMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProtochatMainWindow)
{
    ui->setupUi(this);

    string addr = "127.0.0.1";
    ProtochatSocket sock(addr, 20000);
    if (sock.connect())
        std::cout << "Connected" << std::endl;

    char msg[] = "peepo pog";
    int len = sizeof(msg) - 1;
    sock.send((const byte *) &len, sizeof(int));
    sock.send((const byte *) msg, len);

    sock.receive((byte *) &len, sizeof(int));
    char recv_msg[len + 1];
    recv_msg[len] = '\0';
    sock.receive((byte *) recv_msg, len);
    std::cout << recv_msg << std::endl;

    sock.disconnect();
}

ProtochatMainWindow::~ProtochatMainWindow()
{
    delete ui;
}
