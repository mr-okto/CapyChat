#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "chatclient.h"
#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QHostAddress>


ChatWindow::ChatWindow(QWidget *parent)
    : QWidget(parent)
    , ui_m(new Ui::ChatWindow) // create the elements defined in the .ui file
    , chat_client_m(new ChatClient(this)) // create the chat client
    , chat_model_m(new QStandardItemModel(this)) // create the model to hold the messages
{
    // set up of the .ui file
    ui_m->setupUi(this);
    // the model for the messages will have 1 column
    chat_model_m->insertColumn(0);
    // set the model as the data source vor the list view
    ui_m->chatView->setModel(chat_model_m);
    // connect the signals from the chat client to the slots in this ui
    connect(chat_client_m, &ChatClient::connected, this,
            &ChatWindow::connected_to_server);
    connect(chat_client_m, &ChatClient::logged_in, this, &ChatWindow::logged_in);
    connect(chat_client_m, &ChatClient::message_received, this,
            &ChatWindow::message_received);
    connect(chat_client_m, &ChatClient::error, this, &ChatWindow::error);
    connect(chat_client_m, &ChatClient::user_joined, this,
            &ChatWindow::user_joined);
    connect(chat_client_m, &ChatClient::user_left, this, &ChatWindow::user_left);
    // connect the connect button to a slot that will attempt the connection
    connect(ui_m->connectButton, &QPushButton::clicked, this,
            &ChatWindow::attempt_connection);
    // connect the click of the "send" button and the press of the enter while typing to the slot that sends the message
    connect(ui_m->sendButton, &QPushButton::clicked, this,
            &ChatWindow::send_message);
    connect(ui_m->messageEdit, &QLineEdit::returnPressed, this,
            &ChatWindow::send_message);

    emit attempt_connection();
}

ChatWindow::~ChatWindow()
{
    chat_client_m->disconnect_from_host();
    // delete the elements created from the .ui file
    delete ui_m;
}

void ChatWindow::attempt_connection()
{
    // We ask the user for the address of the server, we use 127.0.0.1 (aka localhost) as default
    const QString hostAddress = QInputDialog::getText(
        this
        , tr("Bootstrap Server")
        , tr("Server Address")
        , QLineEdit::Normal
        , QStringLiteral("127.0.0.1")
    );
    if (hostAddress.isEmpty())
        return; // the user pressed cancel or typed nothing
    // disable the connect button to prevent the user clicking it again
    ui_m->connectButton->setEnabled(false);
    // tell the client to connect to the host
    chat_client_m->connect_to_server(hostAddress);
}

void ChatWindow::connected_to_server()
{
    // once we connected to the server we ask the user for what username they would like to use
    const QString newUsername = QInputDialog::getText(this, tr("Choose Username"), tr("Username"));
    const QString newRoom = QInputDialog::getText(this, tr("Choose Room"), tr("Room"));

    if (newUsername.isEmpty()){
        // if the user clicked cancel or typed nothing, we just disconnect from the server
        return chat_client_m->disconnect_from_host();
    }
    // try to login with the given username
    attempt_login(newUsername, newRoom);
}

void ChatWindow::attempt_login(const QString &user_name, const QString &room_name)
{
    // use the client to attempt a log in with the given username
    chat_client_m->login(user_name, room_name);
}

void ChatWindow::logged_in()
{
    // once we successully log in we enable the ui to display and send messages
    ui_m->sendButton->setEnabled(true);
    ui_m->messageEdit->setEnabled(true);
    ui_m->chatView->setEnabled(true);
    // clear the user name record
    last_username_m.clear();
}

void ChatWindow::message_received(const QString &sender, const QString &text)
{
    // store the index of the new row to append to the model containing the messages
    int newRow = chat_model_m->rowCount();
    // we display a line containing the username only if it's different from the last username we displayed
    if (last_username_m != sender) {
        // store the last displayed username
        last_username_m = sender;
        // create a bold default font
        QFont boldFont;
        boldFont.setBold(true);
        // insert 2 row, one for the message and one for the username
        chat_model_m->insertRows(newRow, 2);
        // store the username in the model
        chat_model_m->setData(chat_model_m->index(newRow, 0), sender + ':');
        // set the alignment for the username
        chat_model_m->setData(chat_model_m->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
        // set the for the username
        chat_model_m->setData(chat_model_m->index(newRow, 0), boldFont, Qt::FontRole);
        ++newRow;
    } else {
        // insert a row for the message
        chat_model_m->insertRow(newRow);
    }
    // store the message in the model
    chat_model_m->setData(chat_model_m->index(newRow, 0), text);
    // set the alignment for the message
    chat_model_m->setData(chat_model_m->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
    // scroll the view to display the new message
    ui_m->chatView->scrollToBottom();
}

void ChatWindow::send_message()
{
    // we use the client to send the message that the user typed
    chat_client_m->send_message(ui_m->messageEdit->text());
    // now we add the message to the list
    // store the index of the new row to append to the model containing the messages
    const int newRow = chat_model_m->rowCount();
    // insert a row for the message
    chat_model_m->insertRow(newRow);
    // store the message in the model
    chat_model_m->setData(chat_model_m->index(newRow, 0), ui_m->messageEdit->text());
    // set the alignment for the message
    chat_model_m->setData(chat_model_m->index(newRow, 0), int(Qt::AlignRight | Qt::AlignVCenter), Qt::TextAlignmentRole);
    // clear the content of the message editor
    ui_m->messageEdit->clear();
    // scroll the view to display the new message
    ui_m->chatView->scrollToBottom();
    // reset the last printed username
    last_username_m.clear();
}


void ChatWindow::user_joined(const QString &username)
{
    // store the index of the new row to append to the model containing the messages
    const int newRow = chat_model_m->rowCount();
    // insert a row
    chat_model_m->insertRow(newRow);
    // store in the model the message to comunicate a user joined
    chat_model_m->setData(chat_model_m->index(newRow, 0), tr("%1 Joined the Chat").arg(username));
    // set the alignment for the text
    chat_model_m->setData(chat_model_m->index(newRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    // set the color for the text
    chat_model_m->setData(chat_model_m->index(newRow, 0), QBrush(Qt::blue), Qt::ForegroundRole);
    // scroll the view to display the new message
    ui_m->chatView->scrollToBottom();
    // reset the last printed username
    last_username_m.clear();
}
void ChatWindow::user_left(const QString &username)
{
    // store the index of the new row to append to the model containing the messages
    const int newRow = chat_model_m->rowCount();
    // insert a row
    chat_model_m->insertRow(newRow);
    // store in the model the message to comunicate a user left
    chat_model_m->setData(chat_model_m->index(newRow, 0), tr("%1 Left the Chat").arg(username));
    // set the alignment for the text
    chat_model_m->setData(chat_model_m->index(newRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    // set the color for the text
    chat_model_m->setData(chat_model_m->index(newRow, 0), QBrush(Qt::red), Qt::ForegroundRole);
    // scroll the view to display the new message
    ui_m->chatView->scrollToBottom();
    // reset the last printed username
    last_username_m.clear();
}

void ChatWindow::error()
{
    QMessageBox::warning(this, tr("Error"), tr("Operation failed, please try again"));

    // enable the button to connect to the server again
    ui_m->connectButton->setEnabled(true);
    // disable the ui to send and display messages
    ui_m->sendButton->setEnabled(false);
    ui_m->messageEdit->setEnabled(false);
    ui_m->chatView->setEnabled(false);
    // reset the last printed username
    last_username_m.clear();
}
