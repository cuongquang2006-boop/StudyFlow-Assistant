#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("AI Chat - Bro Edition 😎");
    resize(500, 600);

    setupUI();

    manager = new QNetworkAccessManager(this);
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    chatBox = new QTextEdit();
    chatBox->setReadOnly(true);

    input = new QLineEdit();
    input->setPlaceholderText("Nhập gì đó đi bro...");

    sendBtn = new QPushButton("Gửi 🚀");

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(input);
    bottomLayout->addWidget(sendBtn);

    mainLayout->addWidget(chatBox);
    mainLayout->addLayout(bottomLayout);

    connect(sendBtn, &QPushButton::clicked,
            this, &MainWindow::sendMessage);

    connect(input, &QLineEdit::returnPressed,
            this, &MainWindow::sendMessage);
}

void MainWindow::sendMessage()
{
    QString text = input->text().trimmed();
    if (text.isEmpty()) return;

    chatBox->append("🧑 Bạn: " + text);
    input->clear();

    sendBtn->setEnabled(false);
    chatBox->append("🤖 AI đang suy nghĩ...");

    QUrl url("http://127.0.0.1:8001/ask"); // 🔥 nhớ port 8001

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["text"] = text;

    QNetworkReply *reply = manager->post(
        request,
        QJsonDocument(json).toJson()
        );

    connect(reply, &QNetworkReply::finished, this, [=]() {

        QByteArray response = reply->readAll();

        sendBtn->setEnabled(true);

        if (reply->error() != QNetworkReply::NoError) {
            chatBox->append("❌ Lỗi mạng: " + reply->errorString());
            reply->deleteLater();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(response);

        if (!doc.isObject()) {
            chatBox->append("❌ JSON lỗi");
            reply->deleteLater();
            return;
        }

        QJsonObject obj = doc.object();

        if (obj.contains("result")) {
            chatBox->append("🤖 AI: " + obj["result"].toString());
        } else {
            chatBox->append("❌ API lỗi");
        }

        chatBox->verticalScrollBar()->setValue(
            chatBox->verticalScrollBar()->maximum()
            );

        reply->deleteLater();
    });
}