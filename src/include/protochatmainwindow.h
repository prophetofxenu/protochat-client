#ifndef PROTOCHATMAINWINDOW_H
#define PROTOCHATMAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class ProtochatMainWindow; }
QT_END_NAMESPACE

class ProtochatMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ProtochatMainWindow(QWidget *parent = nullptr);
    ~ProtochatMainWindow();

private:
    Ui::ProtochatMainWindow *ui;

};

#endif // PROTOCHATMAINWINDOW_H
