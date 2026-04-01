#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QNetworkAccessManager>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void sendMessage();

private:
    QTextEdit *chatBox;
    QLineEdit *input;
    QPushButton *sendBtn;
    QNetworkAccessManager *manager;

    void setupUI();
};

#endif