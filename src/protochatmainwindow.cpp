#include "src/include/protochatmainwindow.h"

#include "src/include/protochatsocket.h"
#include "src/include/protochatregistermessage.h"
#include "ui_protochatmainwindow.h"

#include <iostream>
#include <QTcpSocket>
#include <QTcpServer>
#include <chrono>
#include <string>
using std::string;
#include <thread>
#include <vector>
#include <cstring>

#include <cryptopp/secblock.h>
using CryptoPP::byte;

ProtochatMainWindow::ProtochatMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ProtochatMainWindow)
{
    ui->setupUi(this);

    string addr = "127.0.0.1";
    Protochat::Socket sock(addr, 20000);
    if (sock.connect())
        std::cout << "Connected" << std::endl;


    Protochat::Messages::Register message;
    message.setUsername("Xenu's Prophet");
    message.setPassword("password");
    message.setBio("im gay");
    message.setProfilePicFilename("pfp.png");
    std::vector<std::byte> profilePic;
    profilePic.push_back(static_cast<std::byte>('a'));
    profilePic.push_back(static_cast<std::byte>('b'));
    profilePic.push_back(static_cast<std::byte>('c'));
    message.setProfilePic(profilePic);
    message.setProfilePicCaption("Me when I was less gay");

    std::byte *msgSrl = message.serialize();
    int tmp = message.messageSize();
    sock.send((byte *) &tmp, sizeof(int));
    sock.send((byte *) msgSrl, tmp);

    sock.disconnect();
}

ProtochatMainWindow::~ProtochatMainWindow()
{
    delete ui;
}
