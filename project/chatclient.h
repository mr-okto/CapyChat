#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <opendht.h>

class QHostAddress;
class QJsonDocument;
class ChatClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatClient)
public:
    explicit ChatClient(QObject *parent = nullptr);
public slots:
    void connectToServer(const QString &address);
    void login(const QString &userName, const QString &roomName);
    void sendMessage(const QString &text);
    void disconnectFromHost();
private slots:
    void onReadyRead();
signals:
    void connected();
    void loggedIn();
    void loginError(const QString &reason);
    void disconnected();
    void messageReceived(const QString &sender, const QString &text);
    void error(QAbstractSocket::SocketError socketError);
    void userJoined(const QString &username);
    void userLeft(const QString &username);
private:
    QTcpSocket *m_clientSocket;
    bool m_loggedIn;
    void jsonReceived(const QString &data);
    // Message id generator
    std::uniform_int_distribution<dht::Value::Id> rand_id_m;
    std::mt19937_64 rd {dht::crypto::random_device{}()};
    dht::DhtRunner node;
    dht::InfoHash room_m;
    std::string username_m;
    std::future<size_t> token_m;
};

#endif // CHATCLIENT_H
