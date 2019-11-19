#include <opendht.h>
#include "utils.h"


// Pass port as an argument
int main(int argc,char* argv[]) {
    dht::DhtRunner node;
    long port = (argc >= 2) ? std::strtol(argv[1], nullptr, 10) : 4223;

    // Launch a dht node on a new thread, using a generated RSA key pair
    node.run(port, dht::crypto::generateIdentity(), true);

    // Join the network through any running node,
    // here using a known bootstrap node.
    node.bootstrap("127.0.0.1", "4222");

    std::cout << "OpenDHT node " << node.getNodeId() << " running on port "
              << node.getBoundPort() << std::endl;

    dht::InfoHash key_hash = dht::Hash<HASH_LEN>(TEST_KEY);

    for (int i = 0; i < INT_MAX; i++) {
        std::cout << "\n==========tick " << i << "==========" << std::endl;
        print_info(node, key_hash);

        // Try to get data
        auto val = node.get<std::string>(key_hash);
        std::cout << "Value at " << TEST_KEY << ": ";
        for (const auto& iter : val.get()) {
            std::cout << iter << " ";
        }
        std::cout << std::endl;

        sleep(2.0);
    }
    // wait for dht threads to end
    node.join();
    return 0;
}
