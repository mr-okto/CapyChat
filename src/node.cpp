#include <opendht.h>
#include "utils.h"


// Local chat room node
// "./node" - start bootstrap node at localhost:4222
// "./node <port>" - start client node, bootstrap localhost:4222
// Type message to post message in room
// Press Enter to print node status
int main(int argc, char* argv[]) {
    // Random message id generator
    std::uniform_int_distribution<dht::Value::Id> rand_id;
    std::mt19937_64 rd {dht::crypto::random_device{}()};

    dht::DhtRunner node;
    long port = (argc >= 2) ? std::strtol(argv[1], nullptr, 10) : 4222;

    // Launch a dht node on a new thread, using a generated RSA key pair
    node.run(port, dht::crypto::generateIdentity(), true);

    if (port != 4222) {
        node.bootstrap("127.0.0.1", "4222");
        std::cout << "Bootstrapping localhost:4222" << std::endl;
    }

    std::cout << "OpenDHT node " << node.getNodeId() << " running on port "
              << node.getBoundPort() << std::endl;

    dht::InfoHash key_hash = dht::Hash<HASH_LEN>(TEST_KEY);

    auto token = node.listen<dht::ImMessage>(key_hash, [&](dht::ImMessage&& msg) {
        if (node.getId() != msg.from) {
            std::cout << msg.from.toString() << " - " << msg.id << " - "
                      << msg.msg << std::endl;
        }
        return true;
    });

    std::string msg_text;
    for (int i = 0; i < INT_MAX; i++) {
        std::cout << " > ";
        std::getline(std::cin, msg_text);

        if (!msg_text.empty())
        {
            auto now = std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::now());

            dht::ImMessage new_msg(rand_id(rd), std::move(msg_text), now);
            node.putSigned(key_hash, new_msg, print_publish_status);
        } else {
            std::cout << "\n==========tick " << i << "==========" << std::endl;
            print_info(node, key_hash);
        }
    }

    // Leave room
    node.cancelListen(key_hash, std::move(token));
    // Wait for dht threads to end
    node.join();
    return 0;
}
