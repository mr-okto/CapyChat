#include <opendht.h>
#include "utils.h"
#include "bundled_tools.h"


/* Local chat room node
 *
 * "./node <username>" - start bootstrap node at localhost:4222
 * "./node <port> <username>" - start client node at <port> , bootstrap localhost:4222
 * Type message text to post message in room
*/

int main(int argc, char* argv[]) {
    // Random message id generator
    std::uniform_int_distribution<dht::Value::Id> rand_id;
    std::mt19937_64 rd {dht::crypto::random_device{}()};

    setup_signals();

    dht::DhtRunner node;
    long port = (argc >= 2) ? std::strtol(argv[1], nullptr, 10) : 4222;
    std::string username = (argc >= 3) ? argv[2] : "user";

    // Launch a dht node on a new thread, using a generated RSA key pair
    dht_params params;
    params.port = port;
    params.save_identity = ""; // TODO: Does not work if not empty

    auto dhtConf = get_dgt_config(params);
    node.run(params.port, dhtConf.first, std::move(dhtConf.second));

    std::cout << "OpenDHT node " << node.getNodeId() << " running on port "
              << node.getBoundPort() << std::endl;

    if (port != 4222) {
        node.bootstrap("127.0.0.1", "4222");
        std::cout << "Bootstrapping localhost:4222" << std::endl;
    }

    dht::InfoHash room;
    std::future<size_t> token;

    std::string msg_text;
    bool in_chat = false;
    for (int i = 0; i < INT_MAX; i++) {
        if (not in_chat) {
            std::cout << "Room name: ";
            std::string room_name;
            std::getline(std::cin, room_name);

            room = dht::Hash<HASH_LEN>::get(room_name);
            token = node.listen<dht::ImMessage>(room, [&](dht::ImMessage &&msg)
                    {
                        if (node.getId() != msg.from)
                        {
                            std::cout << "-> message from "
                                      << msg.metadatas.at("username")
                                      << " at " << print_time(msg.date)
                                      << " - " << msg.msg << std::endl;
                        }
                        return true;
                    });
            in_chat = true;
        } else {
            std::getline(std::cin, msg_text);

            if (msg_text == "q") {
                // Leave room
                node.cancelListen(room, token.share());
                in_chat = false;

            } else if (!msg_text.empty()) {
                auto now = std::chrono::system_clock::to_time_t(
                        std::chrono::system_clock::now());

                dht::ImMessage new_msg(rand_id(rd), std::move(msg_text), now);
                new_msg.metadatas.emplace("username", username);
                node.putSigned(room, new_msg, print_publish_status);
            }
        }
    }
    // Wait for dht threads to end
    node.join();
    return 0;
}
