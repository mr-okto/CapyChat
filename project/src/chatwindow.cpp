#include "chatwindow.h"

#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QHostAddress>
#include <QCloseEvent>
#include <QFontDatabase>

#include "ui_chatwindow.h"
#include "chatclient.h"
#include "utils.h"


ChatWindow::ChatWindow(QWidget *parent)
    : QWidget(parent)
    , ui_m(new Ui::ChatWindow)
    , chat_client_m(new ChatClient(this))
    , chat_model_m(new QStandardItemModel(this))
    , room_name_m()
    {
    ui_m->setupUi(this);
    notif_m = new PopUp;
    chat_model_m->insertColumn(0);
    ui_m->chatView->setModel(chat_model_m);

    connect(chat_client_m, &ChatClient::connected, this, &ChatWindow::connected_to_server);
    connect(chat_client_m, &ChatClient::logged_in, this, &ChatWindow::logged_in);
    connect(chat_client_m, &ChatClient::message_received, this,
            &ChatWindow::message_received);
    connect(chat_client_m, &ChatClient::error, this, &ChatWindow::error);
    connect(chat_client_m, &ChatClient::user_joined, this, &ChatWindow::user_joined);
    connect(chat_client_m, &ChatClient::user_left, this, &ChatWindow::user_left);

    connect(ui_m->connectButton, &QPushButton::clicked, this, &ChatWindow::connection_to_other_room);
    connect(ui_m->sendButton, &QPushButton::clicked, this, &ChatWindow::send_message);
    connect(ui_m->messageEdit, &QLineEdit::returnPressed, this, &ChatWindow::send_message);

    emit attempt_connection();
}

void ChatWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton ret;
    ret = QMessageBox::question(this, QApplication::applicationName(), tr("Do you want to close chat?"),
                                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::No)
    {
        event->ignore();
    }
    else if (ret == QMessageBox::Yes)
    {
        chat_client_m->send_message(TYPE_LEFT, "disconnected");
        sleep(1);
        chat_client_m->disconnect_from_host();
        event->accept();
    }
}

ChatWindow::~ChatWindow()
{
    delete ui_m;
}

void ChatWindow::connection_to_other_room()
{
    chat_client_m->send_message(TYPE_LEFT, "disconnected");
    chat_client_m->leave_room();

    connected_to_server();
    chat_model_m->removeRows(0, chat_model_m->rowCount());
}

void ChatWindow::attempt_connection()
{
    const QString hostAddress = QInputDialog::getText(
        this
        , tr("Bootstrap Server")
        , tr("Server Address")
        , QLineEdit::Normal
        , QStringLiteral(BOOTSTRAP_IP)
    );

    const QString newUsername = QInputDialog::getText(
            this, tr("Choose Username"), tr("Username"));

    if (newUsername.isEmpty())
        newUsername.fromStdString("User");

    chat_client_m->connect_to_server(hostAddress, newUsername);

    chat_model_m->removeRows(0, chat_model_m->rowCount());
}

void ChatWindow::connected_to_server()
{
    room_name_m = QInputDialog::getText(this, tr("Choose Room"), tr("Room"));

    if (room_name_m.isEmpty())
    {
        return chat_client_m->disconnect_from_host();
    }
    attempt_login(room_name_m);
}

void ChatWindow::attempt_login(const QString &room_name)
{
    chat_client_m->login(room_name);
}

void ChatWindow::logged_in()
{
    chat_client_m->send_message(TYPE_CONNECTED, "connected");
    ChatWindow::setWindowTitle("Room " + room_name_m);
    ui_m->sendButton->setEnabled(true);
    ui_m->messageEdit->setEnabled(true);
    ui_m->chatView->setEnabled(true);
    setup_time_m = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
    last_username_m.clear();
}

void ChatWindow::message_received(const QString &sender, const QString &type,
                                  const QString &text,
                                  const QString &timestamp)
{
    int newRow = chat_model_m->rowCount();
    if (type == TYPE_CONNECTED)
    {
        user_joined(sender);
        return;
    }

    if (type == TYPE_LEFT)
    {
        user_left(sender);
        return;
    }

    if (type == TYPE_MY_MSG)
    {
        chat_model_m->insertRow(newRow);
        chat_model_m->setData(chat_model_m->index(newRow, 0),
                " (" + timestamp + "): " + text);
        chat_model_m->setData(chat_model_m->index(newRow, 0),
                int(Qt::AlignRight | Qt::AlignVCenter), Qt::TextAlignmentRole);
        ui_m->chatView->scrollToBottom();
        last_username_m.clear();
        return;
    }

    // Show sender username
    if (last_username_m != sender) {
        // store the last displayed username
        last_username_m = sender;
        QFont boldFont;
        font_m.setBold(true);
        chat_model_m->insertRows(newRow, 2);
        chat_model_m->setData(chat_model_m->index(newRow, 0),
                sender);
        chat_model_m->setData(chat_model_m->index(newRow, 0),
                int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
        chat_model_m->setData(chat_model_m->index(newRow, 0),
                font_m, Qt::FontRole);
        ++newRow;
        font_m.setBold(false);
    } else {
        chat_model_m->insertRow(newRow);
    }

    chat_model_m->setData(chat_model_m->index(newRow, 0)," (" + timestamp + "): " + text);
    chat_model_m->setData(chat_model_m->index(newRow, 0),
            int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
    chat_model_m->setData(chat_model_m->index(newRow, 0), font_m, Qt::FontRole);
    ui_m->chatView->scrollToBottom();

    if (std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
            - setup_time_m > 1)
    {
        notif_m->setPopupText(sender + ": " + text);
        notif_m->show();
    }
}

void ChatWindow::send_message()
{
    auto now = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
    auto timestamp = QString::fromStdString(print_time(now));
    chat_client_m->send_message(TYPE_MSG, ui_m->messageEdit->text());
    ui_m->messageEdit->clear();
}

void ChatWindow::user_joined(const QString &username)
{
    const int newRow = chat_model_m->rowCount();
    chat_model_m->insertRow(newRow);
    chat_model_m->setData(chat_model_m->index(newRow, 0), tr("%1 in chat now").arg(username));
    chat_model_m->setData(chat_model_m->index(newRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    chat_model_m->setData(chat_model_m->index(newRow, 0), QBrush(Qt::blue), Qt::ForegroundRole);
    ui_m->chatView->scrollToBottom();
    last_username_m.clear();
}

void ChatWindow::user_left(const QString &username)
{
    const int newRow = chat_model_m->rowCount();
    chat_model_m->insertRow(newRow);
    chat_model_m->setData(chat_model_m->index(newRow, 0), tr("%1 Left the Chat").arg(username));
    chat_model_m->setData(chat_model_m->index(newRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    chat_model_m->setData(chat_model_m->index(newRow, 0), QBrush(Qt::red), Qt::ForegroundRole);
    ui_m->chatView->scrollToBottom();
    last_username_m.clear();
}

void ChatWindow::error()
{
    QMessageBox::warning(this, tr("Error"), tr("Operation failed, please try again"));

    ui_m->connectButton->setEnabled(true);

    ui_m->sendButton->setEnabled(false);
    ui_m->messageEdit->setEnabled(false);
    ui_m->chatView->setEnabled(false);

    last_username_m.clear();
}
