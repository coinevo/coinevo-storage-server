# coinevo-storage-server
Storage server for Coinevo Service Nodes

Requirements:
* Boost >= 1.66 (for boost.beast)
* OpenSSL >= 1.1.1a (for X25519 curves)
* sodium (for ed25119 to curve25519 conversion)

```
make
./httpserver 127.0.0.1 8080
```

The paths for Boost and OpenSSL can be specified by exporting the variables in the terminal before running `make`:
```
export OPENSSL_ROOT_DIR = ...
export BOOST_ROOT= ...
```

Then using something like Postman (https://www.getpostman.com/) you can hit the API:

# post data
```
HTTP POST http://127.0.0.1/store
body: "hello world"
headers:
- X-Coinevo-recipient: "mypubkey"
- X-Coinevo-ttl: "86400"
- X-Coinevo-timestamp: "1540860811000"
- X-Coinevo-pow-nonce: "xxxx..."
```
# get data
```
HTTP GET http://127.0.0.1/retrieve
headers:
- X-Coinevo-recipient: "mypubkey"
- X-Coinevo-last-hash: "" (optional)
```

# unit tests
```
mkdir build_test
cd build_test
cmake ../unit_test -DBOOST_ROOT="path to boost" -DOPENSSL_ROOT_DIR="path to openssl"
cmake --build .
./Test --log_level=all
```