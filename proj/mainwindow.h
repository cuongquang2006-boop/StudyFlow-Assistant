#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

class ChatBubble;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private:
    void setupUI();

    void sendMessage();
    void addMessage(const QString& text, bool isUser);
    void typeMessage(ChatBubble* bubble, const QString& fullText);

    void smoothScroll();
    void stopRequest();

    void setupInputAnimation();

    void animateInputShadow(
        qreal blur,
        QPointF offset
        );

    void animateInputContainer(
        qreal blur,
        QPointF offset,
        int moveY);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:

    QRect originalInputGeometry;
    QScrollArea* scrollArea;
    QWidget* chatContainer;
    QVBoxLayout* chatLayout;

    QWidget* inputContainer;
    QLineEdit* input;
    QPushButton* sendBtn;

    QNetworkAccessManager* manager;
    QNetworkReply* currentReply = nullptr;

    bool isStopped = false;
    QTimer* typingTimer = nullptr;

    ChatBubble* currentBubble = nullptr;
    QString currentFullText;
};

#endif 
