#include "chatwindow.h"

#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QHostAddress>
#include <QCloseEvent>
#include <QFontDatabase>

#include <iostream>
#include <fstream>
#include <string>


#include "ui_chatwindow.h"
#include "chatclient.h"
#include "utils.h"


ChatWindow::ChatWindow(QWidget *parent)
    : QWidget(parent)
    , ui_m(new Ui::ChatWindow)
    , chat_client_m(new ChatClient(this))
    , chat_model_m(new QStandardItemModel(this))
    {
    ui_m->setupUi(this);
    chat_model_m->insertColumn(0);
    ui_m->chatView->setModel(chat_model_m);
    ui_m->StatusBar->setAlignment(Qt::AlignCenter);

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
    std::fstream file;

    std::string ip;
    std::string nickname;

    QString hostAddress;
    QString newUsername;

    file.open(AUTO_INPUT_DATA, std::ios::in);

    if (file.is_open() && file >> ip && file >> nickname)
    {
        file.close();

        hostAddress = QInputDialog::getText(
            this
            , tr("Bootstrap Server")
            , tr("Server Address")
            , QLineEdit::Normal
            , QString::fromStdString(ip));
        if (hostAddress.isEmpty())
        {
            ui_m->StatusBar->setText("Choosen nick - " + StatusUsername + "    " + "Room name - ");
            return;
        }

        newUsername = QInputDialog::getText(
                this
                , tr("Choose Username")
                , tr("Username")
                , QLineEdit::Normal
                , QString::fromStdString(nickname));
        if (newUsername.isEmpty())
        {
            ui_m->StatusBar->setText("Choosen nick - " + StatusUsername + "    " + "Room name - ");
            return;
        }

        file.open(AUTO_INPUT_DATA, std::ios::out);

        file << hostAddress.toStdString() << std::endl;
        file << newUsername.toStdString() << std::endl;

        file.close();
    }
    else
    {
        hostAddress = QInputDialog::getText(
            this
            , tr("Bootstrap Server")
            , tr("Server Address")
        );
        if (hostAddress.isEmpty())
        {
            ui_m->StatusBar->setText("Choosen nick - " + StatusUsername + "    " + "Room name - ");
            return;
        }

        newUsername = QInputDialog::getText(
                this
                , tr("Choose Username")
                , tr("Username")
        );
        if (newUsername.isEmpty())
        {
            ui_m->StatusBar->setText("Choosen nick - " + StatusUsername + "    " + "Room name - ");
            return;
        }

        file.open(AUTO_INPUT_DATA, std::ios::out);

        file << hostAddress.toStdString() << std::endl;
        file << newUsername.toStdString() << std::endl;

        file.close();
    }

    ui_m->StatusBar->setText("Choosen nick - " + newUsername);

    StatusUsername = newUsername;

    chat_client_m->connect_to_server(hostAddress, newUsername);

    chat_model_m->removeRows(0, chat_model_m->rowCount());

}

void ChatWindow::connected_to_server()
{
    const QString newRoom = QInputDialog::getText(this, tr("Choose Room"), tr("Room"));

    if (newRoom.isEmpty())
    {
        ui_m->StatusBar->setText("Choosen nick - " + StatusUsername + "    " + "Room name - " + newRoom);
        return chat_client_m->disconnect_from_host();
    }
    attempt_login(newRoom);

    ui_m->StatusBar->setText("Choosen nick - " + StatusUsername + "    " + "Room name - " + newRoom);
}
    
void ChatWindow::attempt_login(const QString &room_name)
{
    chat_client_m->login(room_name);
}

void ChatWindow::logged_in()
{
    chat_client_m->send_message(TYPE_CONNECTED, "connected");
    ui_m->sendButton->setEnabled(true);
    ui_m->messageEdit->setEnabled(true);
    ui_m->chatView->setEnabled(true);
    ui_m->StatusBar->setEnabled(true);
    ui_m->StatusBar->setAlignment(Qt::AlignCenter);
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

    // Show sender username
    if (last_username_m != sender) {
        // store the last displayed username
        last_username_m = sender;
        QFont boldFont;
        font_m.setBold(true);
        chat_model_m->insertRows(newRow, 2);
        chat_model_m->setData(chat_model_m->index(newRow, 0),
                sender + " (" + timestamp  + ")");
        chat_model_m->setData(chat_model_m->index(newRow, 0),
                int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
        chat_model_m->setData(chat_model_m->index(newRow, 0),
                font_m, Qt::FontRole);
        ++newRow;
        font_m.setBold(false);
    } else {
        chat_model_m->insertRow(newRow);
    }
    chat_model_m->setData(chat_model_m->index(newRow, 0), text);
    chat_model_m->setData(chat_model_m->index(newRow, 0),
            int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
    chat_model_m->setData(chat_model_m->index(newRow, 0), font_m, Qt::FontRole);
    ui_m->chatView->scrollToBottom();
}

void ChatWindow::send_message()
{
    chat_client_m->send_message(TYPE_MSG, ui_m->messageEdit->text());
    const int newRow = chat_model_m->rowCount();
    chat_model_m->insertRow(newRow);
    chat_model_m->setData(chat_model_m->index(newRow, 0), ui_m->messageEdit->text());
    chat_model_m->setData(chat_model_m->index(newRow, 0),
            int(Qt::AlignRight | Qt::AlignVCenter), Qt::TextAlignmentRole);
    ui_m->messageEdit->clear();
    ui_m->chatView->scrollToBottom();
    last_username_m.clear();
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
