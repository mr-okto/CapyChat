#include "chatclient.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "bundled_tools.h"
#include "utils.h"

ChatClient::ChatClient(QObject *parent)
    : QObject(parent)
    , m_clientSocket(new QTcpSocket(this))
    , m_loggedIn(false)
{
}

void ChatClient::login(const QString &userName, const QString &roomName)
{
    room_m = dht::Hash<HASH_LEN>::get(roomName.toStdString());
    token_m = node.listen<dht::ImMessage>(room_m, [&](dht::ImMessage &&msg)
    {
      if (node.getId() != msg.from)
      {
          auto from_user = QString::fromStdString(
                  msg.metadatas.at("username"));
          auto msg_data = QString::fromStdString(msg.msg);
          emit messageReceived(from_user, msg_data);
      }
      return true;
    });
    m_loggedIn = true;
    username_m = userName.toStdString();
    emit loggedIn();
}

void ChatClient::sendMessage(const QString &text)
{
    if (text.isEmpty())
        return; // We don't send empty messages

    auto now = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
    dht::ImMessage new_msg(rand_id_m(rd), std::move(text.toStdString()), now);
    new_msg.metadatas.emplace("username", username_m);
    node.putSigned(room_m, new_msg, print_publish_status);
}

void ChatClient::disconnectFromHost()
{
    node.join();
    emit disconnected();
}

void ChatClient::jsonReceived(const QString &data)
{
//    // actions depend on the type of message
//    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
//    if (typeVal.isNull() || !typeVal.isString())
//        return; // a message with no type was received so we just ignore it
//    if (typeVal.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) == 0) { //It's a login message
//        if (m_loggedIn)
//            return; // if we are already logged in we ignore
//        // the success field will contain the result of our attempt to login
//        const QJsonValue resultVal = docObj.value(QLatin1String("success"));
//        if (resultVal.isNull() || !resultVal.isBool())
//            return; // the message had no success field so we ignore
//        const bool loginSuccess = resultVal.toBool();
//        if (loginSuccess) {
//            // we logged in succesfully and we notify it via the loggedIn signal
//            emit loggedIn();
//            return;
//        }
//        // the login attempt failed, we extract the reason of the failure from the JSON
//        // and notify it via the loginError signal
//        const QJsonValue reasonVal = docObj.value(QLatin1String("reason"));
//        emit loginError(reasonVal.toString());
//    } else if (typeVal.toString().compare(QLatin1String("message"), Qt::CaseInsensitive) == 0) { //It's a chat message
//        // we extract the text field containing the chat text
//        const QJsonValue textVal = docObj.value(QLatin1String("text"));
//        // we extract the sender field containing the username of the sender
//        const QJsonValue senderVal = docObj.value(QLatin1String("sender"));
//        if (textVal.isNull() || !textVal.isString())
//            return; // the text field was invalid so we ignore
//        if (senderVal.isNull() || !senderVal.isString())
//            return; // the sender field was invalid so we ignore
//        // we notify a new message was received via the messageReceived signal




        emit messageReceived(data, data);



//    } else if (typeVal.toString().compare(QLatin1String("newuser"), Qt::CaseInsensitive) == 0) { // A user joined the chat
//        // we extract the username of the new user
//        const QJsonValue usernameVal = docObj.value(QLatin1String("username"));
//        if (usernameVal.isNull() || !usernameVal.isString())
//            return; // the username was invalid so we ignore
//        // we notify of the new user via the userJoined signal
//        emit userJoined(usernameVal.toString());
//    } else if (typeVal.toString().compare(QLatin1String("userdisconnected"), Qt::CaseInsensitive) == 0) { // A user left the chat
//         // we extract the username of the new user
//        const QJsonValue usernameVal = docObj.value(QLatin1String("username"));
//        if (usernameVal.isNull() || !usernameVal.isString())
//            return; // the username was invalid so we ignore
//        // we notify of the user disconnection the userLeft signal
//        emit userLeft(usernameVal.toString());
//    }
}

void ChatClient::connectToServer(const QString &address)
{
    dht_params params;
    params.port = 4222;
    params.log = false;

    auto dhtConf = get_dgt_config(params);
    try {
        node.run(params.port, dhtConf.first, std::move(dhtConf.second));
    } catch (dht::DhtException& e) {
        node.run(0, dhtConf.first, std::move(dhtConf.second));
    }

    std::cout << "OpenDHT node " << node.getNodeId() << " running on port "
              << node.getBoundPort() << std::endl;

    node.bootstrap(address.toStdString(), "4222");
    std::cout << "Bootstrapping" << address.toStdString() << ":4222" << std::endl;
    emit connected();
}

void ChatClient::onReadyRead()
{
    // prepare a container to hold the UTF-8 encoded JSON we receive from the socket
    QByteArray jsonData;
    // create a QDataStream operating on the socket
    QDataStream socketStream(m_clientSocket);
    // set the version so that programs compiled with different versions of Qt can agree on how to serialise
    socketStream.setVersion(QDataStream::Qt_5_7);
    // start an infinite loop
    for (;;) {
        // we start a transaction so we can revert to the previous state in case we try to read more data than is available on the socket
        socketStream.startTransaction();
        // we try to read the JSON data
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            // we successfully read some data
            // we now need to make sure it's in fact a valid JSON
            QJsonParseError parseError;
            // we try to create a json document with the data we received
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                // if the data was indeed valid JSON
                if (jsonDoc.isObject()) // and is a JSON object
                    jsonReceived("Hello"); // parse the JSON
            }
            // loop and try to read more JSONs if they are available
        } else {
            // the read failed, the socket goes automatically back to the state it was in before the transaction started
            // we just exit the loop and wait for more data to become available
            break;
        }
    }
}
