#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <opendht.h>

class ChatClient : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ChatClient)
public:
    explicit ChatClient(QObject *parent = nullptr);
public slots:
    void connect_to_server(const QString &address, const QString &username);
    void login(const QString &room_name);
    void send_message(const QString &type, const QString &text);
    void leave_room();
    void disconnect_from_host();
signals:
    void connected();
    void logged_in();
    void room_left();
    void message_received(const QString &sender, const QString &type,
                          const QString &text, const QString &timestamp);
    void error();
    void user_joined(const QString &username);
    void user_left(const QString &username);
private:
    bool logged_in_m;
    // Message id generator
    std::uniform_int_distribution<dht::Value::Id> rand_id_m;
    std::mt19937_64 rd {dht::crypto::random_device{}()};
    dht::DhtRunner node;
    dht::InfoHash room_m;
    std::string username_m;
    std::future<size_t> token_m;
};

#endif // CHATCLIENT_H
