#include <opendht.h>
#include <vector>
#include <cstdint>

bool not_lambda(const std::vector<std::shared_ptr<dht::Value>>& values) {
// Callback called when values are found
    std::cout << "In lambda" << std::endl;
    for (const auto& value : values)
    std::cout << "Found value: " << *value << std::endl;
    return false; // return false to stop the search
}

int main()
{

    dht::DhtRunner node;

    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port 4222.
    node.run(4222, dht::crypto::generateIdentity(), true);

    // Join the network through any running node,
    // here using a known bootstrap node.
    node.bootstrap("bootstrap.ring.cx", "4222");

    // put some data on the dht
    std::string some_data("Message");
    dht::InfoHash key = dht::Hash<HASH_LEN>("unique_key");
    node.put(key, some_data);

    // get data
    auto val = node.get<std::string>(key);  // std::future
    for (const auto& iter : val.get()) {
        std::cout << iter << std::endl;
    }
    // wait for dht threads to end
    node.join();
    return 0;
}
