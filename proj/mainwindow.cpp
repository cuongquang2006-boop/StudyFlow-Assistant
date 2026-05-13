#include "mainwindow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QApplication>

#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>

class ChatBubble : public QWidget
{
public:
    QLabel* label;

    ChatBubble(const QString& text, bool isUser)
    {
        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(14, 6, 14, 6);

        auto* bubble = new QWidget();
        auto* bubbleLayout = new QVBoxLayout(bubble);

        bubbleLayout->setContentsMargins(18, 14, 18, 14);

        label = new QLabel(text);

        label->setWordWrap(true);
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        label->setMaximumWidth(isUser ? 560 : 760);

        bubbleLayout->addWidget(label);

        if(isUser)
        {
            bubble->setStyleSheet(R"(
                background:qlineargradient(
                    x1:0,y1:0,x2:1,y2:1,
                    stop:0 #3b82f6,
                    stop:1 #2563eb
                );
                border-radius:22px;
            )");

            label->setStyleSheet(R"(
                color:white;
                border:none;
                font-size:16px;
                font-weight:500;
                background:transparent;
            )");

            layout->addStretch();
            layout->addWidget(bubble);
        }

        else
        {
            bubble->setStyleSheet(R"(
                background:transparent;
                border:none;
            )");

            bubbleLayout->setContentsMargins(6, 6, 6, 6);

            label->setStyleSheet(R"(
                color:#f8fafc;
                border:none;
                font-size:16px;
                font-weight:400;
                background:transparent;
            )");

            label->setSizePolicy(
                QSizePolicy::Expanding,
                QSizePolicy::Preferred
            );

            bubble->setSizePolicy(
                QSizePolicy::Expanding,
                QSizePolicy::Preferred
            );

            layout->addWidget(bubble, 1);
        }
    }

    void setText(const QString& text)
    {
        label->setText(text);
    }
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    resize(1100, 760);

    setMinimumSize(820, 600);

    setWindowTitle("Mini ChatGPT");

    setupUI();

    manager = new QNetworkAccessManager(this);
}

void MainWindow::setupUI()
{
    setStyleSheet(R"(
        QMainWindow {
            background:qlineargradient(
                x1:0,y1:0,x2:1,y2:1,
                stop:0 #0f172a,
                stop:1 #111827
            );
        }

        QLabel {
            color:white;
            border-radius:6px;
        }

        QLineEdit {
            background:transparent;
            border:none;
            color:white;
            font-size:15px;
            padding-left:10px;
        }

        QScrollArea {
            border:none;
            background:transparent;
        }

        QScrollBar:vertical {
            background:transparent;
            width:10px;
            margin:4px 2px 4px 2px;
        }

        QScrollBar::handle:vertical {
            background:rgba(255,255,255,90);
            border-radius:5px;
            min-height:40px;
        }

        QScrollBar::handle:vertical:hover {
            background:rgba(255,255,255,150);
        }

        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical {
            height:0px;
        }
    )");

    auto* central = new QWidget(this);

    setCentralWidget(central);

    auto* rootLayout = new QVBoxLayout(central);

    rootLayout->setContentsMargins(18, 18, 18, 18);
    rootLayout->setSpacing(14);

    auto* header = new QWidget();

    header->setFixedHeight(80);

    header->setStyleSheet(R"(
        background:rgba(255,255,255,0.05);
        border:1px solid rgba(255,255,255,0.08);
        border-radius:24px;
    )");

    auto* headerLayout = new QHBoxLayout(header);

    headerLayout->setContentsMargins(22, 12, 22, 12);

    auto* avatar = new QLabel("🤖");

    avatar->setFixedSize(52, 52);

    avatar->setAlignment(Qt::AlignCenter);

    avatar->setStyleSheet(R"(
        font-size:30px;
        background:#2563eb;
        border-radius:22px;
        padding:8px;
    )");

    auto* titleLayout = new QVBoxLayout();

    auto* title = new QLabel("Study Bot");

    title->setStyleSheet(R"(
        font-size:20px;
        font-weight:700;
        color:white;
    )");

    auto* subtitle = new QLabel("AI Assistant Online");

    subtitle->setStyleSheet(R"(
        font-size:13px;
        color:rgba(255,255,255,0.6);
    )");

    titleLayout->addWidget(title);
    titleLayout->addWidget(subtitle);

    headerLayout->addWidget(avatar);
    headerLayout->addSpacing(10);
    headerLayout->addLayout(titleLayout);
    headerLayout->addStretch();

    scrollArea = new QScrollArea();

    scrollArea->setWidgetResizable(true);

    chatContainer = new QWidget();

    chatLayout = new QVBoxLayout(chatContainer);

    chatLayout->setAlignment(Qt::AlignTop);
    chatLayout->setSpacing(14);
    chatLayout->setContentsMargins(8, 12, 8, 12);

    scrollArea->setWidget(chatContainer);

    inputContainer = new QWidget();

    inputContainer->setFixedHeight(90);

    inputContainer->setStyleSheet(R"(
        background:rgba(255,255,255,0.06);
        border:1px solid rgba(255,255,255,0.08);
        border-radius:28px;
    )");

    auto* inputLayout = new QHBoxLayout(inputContainer);

    inputLayout->setContentsMargins(18, 12, 12, 12);
    inputLayout->setSpacing(12);

    input = new QLineEdit();

    input->setPlaceholderText("Send something, bro...");

    input->setMinimumHeight(54);

    input->setStyleSheet(R"(
        QLineEdit {
            background:rgba(255,255,255,0.08);
            border:2px solid transparent;
            border-radius:20px;
            padding:0 14px;
            color:white;
            font-size:15px;
        }

        QLineEdit:focus {
            border:2px solid #60a5fa;
            background:rgba(255,255,255,0.12);
        }
    )");

    sendBtn = new QPushButton("➜");

    sendBtn->setCursor(Qt::PointingHandCursor);

    sendBtn->setFixedSize(54, 54);

    sendBtn->setStyleSheet(R"(
        QPushButton {
            background:qlineargradient(
                x1:0,y1:0,x2:1,y2:1,
                stop:0 #3b82f6,
                stop:1 #2563eb
            );
            color:white;
            border:none;
            border-radius:27px;
            font-size:22px;
            font-weight:bold;
        }

        QPushButton:hover {
            background:qlineargradient(
                x1:0,y1:0,x2:1,y2:1,
                stop:0 #60a5fa,
                stop:1 #3b82f6
            );
        }

        QPushButton:pressed {
            padding-top:3px;
            background:#1d4ed8;
        }
    )");

    sendBtn->installEventFilter(this);

    inputLayout->addWidget(input);
    inputLayout->addWidget(sendBtn);

    setupInputAnimation();

    rootLayout->addWidget(header);
    rootLayout->addWidget(scrollArea);
    rootLayout->addWidget(inputContainer);

    QTimer::singleShot(0, this, [=]()
    {
        originalInputGeometry = inputContainer->geometry();
    });

    QTimer::singleShot(250, this, [=]()
    {
        addMessage("Yo bro, what software are building today?", false);
    });

    connect(sendBtn, &QPushButton::clicked, this, [=]()
    {
        if(currentReply)
            stopRequest();
        else
            sendMessage();
    });

    connect(
        input,
        &QLineEdit::returnPressed,
        this,
        &MainWindow::sendMessage
    );
}

void MainWindow::addMessage(const QString& text, bool isUser)
{
    auto* bubble = new ChatBubble(text, isUser);

    auto* effect = new QGraphicsOpacityEffect(bubble);

    bubble->setGraphicsEffect(effect);

    chatLayout->addWidget(bubble);

    auto* fade = new QPropertyAnimation(effect, "opacity");

    fade->setDuration(200);
    fade->setStartValue(0);
    fade->setEndValue(1);

    fade->start(QAbstractAnimation::DeleteWhenStopped);

    QApplication::processEvents();

    smoothScroll();
}

void MainWindow::typeMessage(ChatBubble* bubble, const QString& fullText)
{
    currentBubble = bubble;

    currentFullText = fullText;

    typingTimer = new QTimer(this);

    int index = 0;

    connect(typingTimer, &QTimer::timeout, this, [=]() mutable
    {
        if(isStopped)
        {
            bubble->setText(fullText);

            typingTimer->stop();

            return;
        }

        index++;

        bubble->setText(fullText.left(index));

        smoothScroll();

        if(index >= fullText.length())
            typingTimer->stop();
    });

    typingTimer->start(10);
}

void MainWindow::smoothScroll()
{
    auto* bar = scrollArea->verticalScrollBar();

    auto* anim = new QPropertyAnimation(bar, "value");

    anim->setDuration(300);
    anim->setStartValue(bar->value());
    anim->setEndValue(bar->maximum());
    anim->setEasingCurve(QEasingCurve::OutQuart);

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::sendMessage()
{
    QString text = input->text().trimmed();

    if(text.isEmpty())
        return;

    isStopped = false;

    addMessage(text, true);

    input->clear();

    sendBtn->setText("⏹");

    QNetworkRequest request(
        QUrl("https://cloud-ai-flks.onrender.com/ask")
    );

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
    );

    QJsonObject json;

    json["text"] = text;

    QNetworkReply* reply =
        manager->post(
            request,
            QJsonDocument(json).toJson()
        );

    currentReply = reply;

    connect(reply, &QNetworkReply::finished, this, [=]()
    {
        sendBtn->setText("↑");

        if(reply->error() == QNetworkReply::OperationCanceledError)
            return;

        if(isStopped)
            return;

        QByteArray response = reply->readAll();

        QJsonDocument doc =
            QJsonDocument::fromJson(response);

        if(doc.isObject())
        {
            QString result =
                doc.object()["result"].toString();

            auto* bubble =
                new ChatBubble("", false);

            chatLayout->addWidget(bubble);

            typeMessage(bubble, result);
        }
    });
}

void MainWindow::stopRequest()
{
    isStopped = true;

    if(currentReply)
    {
        currentReply->abort();
        currentReply = nullptr;
    }

    if(typingTimer)
        typingTimer->stop();

    if(currentBubble)
        currentBubble->setText(currentFullText);

    sendBtn->setText("↑");
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    if(obj == sendBtn)
    {
        if(event->type() == QEvent::Enter)
        {
            sendBtn->setStyleSheet(R"(
                QPushButton {
                    background:#60a5fa;
                    color:white;
                    border:none;
                    border-radius:27px;
                    font-size:22px;
                    font-weight:bold;
                }
            )");
        }

        else if(event->type() == QEvent::Leave)
        {
            sendBtn->setStyleSheet(R"(
                QPushButton {
                    background:#3b82f6;
                    color:white;
                    border:none;
                    border-radius:27px;
                    font-size:22px;
                    font-weight:bold;
                }
            )");
        }
    }

    else if(obj == inputContainer)
    {
        if(event->type() == QEvent::Enter)
        {
            animateInputContainer(
                40,
                QPointF(0, 10),
                -3
            );
        }

        else if(event->type() == QEvent::Leave)
        {
            animateInputContainer(
                20,
                QPointF(0, 4),
                0
            );
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::setupInputAnimation()
{
    inputContainer->setAttribute(Qt::WA_Hover);

    inputContainer->installEventFilter(this);

    auto* shadow =
        new QGraphicsDropShadowEffect(this);

    shadow->setBlurRadius(20);

    shadow->setOffset(0, 4);

    shadow->setColor(QColor(96,165,250,120));

    inputContainer->setGraphicsEffect(shadow);
}

void MainWindow::animateInputShadow(
    qreal blur,
    QPointF offset
)
{
    auto* shadow =
        qobject_cast<QGraphicsDropShadowEffect*>(
            inputContainer->graphicsEffect()
        );

    if(!shadow)
        return;

    auto* blurAnim =
        new QPropertyAnimation(
            shadow,
            "blurRadius"
        );

    blurAnim->setDuration(180);
    blurAnim->setStartValue(shadow->blurRadius());
    blurAnim->setEndValue(blur);

    auto* offsetAnim =
        new QPropertyAnimation(
            shadow,
            "offset"
        );

    offsetAnim->setDuration(180);
    offsetAnim->setStartValue(shadow->offset());
    offsetAnim->setEndValue(offset);

    blurAnim->start(QAbstractAnimation::DeleteWhenStopped);

    offsetAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::animateInputContainer(
    qreal blur,
    QPointF offset,
    int moveY
)
{
    auto* shadow =
        qobject_cast<QGraphicsDropShadowEffect*>(
            inputContainer->graphicsEffect()
        );

    if(!shadow)
        return;

    auto* blurAnim =
        new QPropertyAnimation(
            shadow,
            "blurRadius"
        );

    blurAnim->setDuration(180);
    blurAnim->setStartValue(shadow->blurRadius());
    blurAnim->setEndValue(blur);

    auto* offsetAnim =
        new QPropertyAnimation(
            shadow,
            "offset"
        );

    offsetAnim->setDuration(180);
    offsetAnim->setStartValue(shadow->offset());
    offsetAnim->setEndValue(offset);

    auto* moveAnim =
        new QPropertyAnimation(
            inputContainer,
            "geometry"
        );

    QRect target = originalInputGeometry;

    target.moveTop(
        originalInputGeometry.y() + moveY
    );

    moveAnim->setDuration(180);

    moveAnim->setStartValue(
        inputContainer->geometry()
    );

    moveAnim->setEndValue(target);

    blurAnim->start(QAbstractAnimation::DeleteWhenStopped);

    offsetAnim->start(QAbstractAnimation::DeleteWhenStopped);

    moveAnim->start(QAbstractAnimation::DeleteWhenStopped);
}
