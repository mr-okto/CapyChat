#include "chatclient.h"
#include <QDataStream>

#include "node_tools.h"
#include "utils.h"

ChatClient::ChatClient(QObject *parent)
    : QObject(parent)
    , logged_in_m(false) {}

void ChatClient::login(const QString &room_name)
{
    room_m = dht::Hash<HASH_LEN>::get(room_name.toStdString());
    token_m = node.listen<dht::ImMessage>(room_m, [&](dht::ImMessage &&msg)
    {
      if (node.getId() != msg.from)
      {
          auto from_user = QString::fromStdString(
                  msg.metadatas.at("username"));
          auto msg_type = QString::fromStdString(
                  msg.metadatas.at("type"));
          auto msg_data = QString::fromStdString(msg.msg);
          auto timestamp = QString::fromStdString(print_time(msg.date));
          emit message_received(from_user, msg_type, msg_data, timestamp);
      }
      return true;
    });
    logged_in_m = true;
    emit logged_in();
}

void ChatClient::send_message(const QString &type, const QString &text)
{
    if (text.isEmpty())
        return;
    auto now = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
    dht::ImMessage new_msg(rand_id_m(rd), std::move(text.toStdString()), now);
    new_msg.metadatas.emplace("username", username_m);
    new_msg.metadatas.emplace("type", type.toStdString());
    node.putSigned(room_m, new_msg, print_publish_status, true);
}

void ChatClient::leave_room()
{
    node.cancelListen(room_m, std::move(token_m));
    emit room_left();
}

void ChatClient::disconnect_from_host()
{
    node.join();
}

void
ChatClient::connect_to_server(const QString &address, const QString &username)
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

    std::string node_id = node.getNodeId().toString();

    username_m = username.toStdString() + " [" + node_id.substr(node_id.length() - 4) + "]";


    std::cout << "OpenDHT node " << node.getNodeId() << " running on port "
              << node.getBoundPort() << std::endl;

    node.bootstrap(address.toStdString(), "4222");
    std::cout << "Bootstrapping " << address.toStdString() << ":4222" << std::endl;
    emit connected();
}

