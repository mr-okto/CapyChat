#include <opendht.h>
#include "utils.h"
#include "dht_config.h"


/* Local chat room node
 * "./node <port> - launch bootstrap node
*/

int main(int argc, char* argv[]) {
    dht::DhtRunner node;

    long port = (argc >= 2) ? std::strtol(argv[1], nullptr, 10) : 4222;

    // Launch a dht node on a new thread, using a generated RSA key pair
    dht_params params;
    params.port = port;
    params.log = false;
    params.persist_path = "node.state";

    auto dhtConf = get_dgt_config(params);
    node.run(params.port, dhtConf.first, std::move(dhtConf.second));

    std::cout << "OpenDHT node " << node.getNodeId() << " running on port "
              << node.getBoundPort() << std::endl;

    std::string msg_text;
    bool exit_now = false;
    while (not exit_now) {
            std::getline(std::cin, msg_text);
            if (msg_text == "q" || msg_text == "exit") {
                exit_now = true;

            } else {
                print_info(node);
            }
    }
    // Ending node work
    std::condition_variable cv;
    std::mutex m;
    bool done {false};

    node.shutdown([&]()
                   {
                       done = true;
                       cv.notify_all();
                   });

    // wait for shutdown
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [&](){ return done; });

    node.join();
    return 0;
}
