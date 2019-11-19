#include <opendht.h>
#include "utils.h"


int main()
{
    dht::DhtRunner node;
    // Launch a dht node on a new thread, using a generated RSA key pair
    node.run(4222, dht::crypto::generateIdentity());


    std::cout << "OpenDHT HOST node " << node.getNodeId() << " running on port "
              << node.getBoundPort() << std::endl;

    // Put some data on the dht
    std::string some_data("Message");
    dht::InfoHash key = dht::Hash<HASH_LEN>(TEST_KEY);
    node.put(key, some_data);

    for (int i = 0; i < INT_MAX; i++) {
        std::cout << "\n==========tick " << i << "==========" << std::endl;
        print_info(node, key);
        sleep(10.0);
    }
    // wait for dht threads to end
    node.join();
    return 0;
}
