# TCP Chat App

A minimal two-person chat application written in C++ using raw TCP sockets. A central server brokers messages between exactly two clients in real time.

## How it works

1. The server starts and listens on port **8080**.
2. The first client connects and waits while the server holds the connection open.
3. Once a second client connects, both are notified and can begin chatting.
4. Each client's messages are relayed to the other with their username prepended.
5. When either client disconnects, the other is notified and the session ends.

## Requirements

- Linux (uses POSIX sockets: `<sys/socket.h>`, `<netinet/in.h>`, etc.)
- C++11 or later
- `g++` with pthread support

## Build

```bash
g++ -std=c++11 -pthread -o server server.cpp
g++ -std=c++11 -pthread -o client client.cpp
```

## Usage

**Start the server** (on the host machine):
```bash
./server
# Waiting for 2 clients on port 8080...
```

**Connect two clients** (on the same machine or over a network):
```bash
./client                  # defaults to 127.0.0.1
./client <server-ip>      # connect to a remote server
```

Each client will be prompted for a username, then the chat begins once both are connected. Type a message and press **Enter** to send. Press **Ctrl+C** or type `quit` to exit.

## Example session

```
$ ./client 192.168.1.10
Enter your username: Alice
Connected. Type messages and press Enter. Press CNTRL + C to exit.
Waiting for second client...
Bob joined. Start chatting!
Bob: hey Alice!
hey Bob!
```

## Project structure

```
.
├── server.cpp   # TCP server — accepts two clients and relays messages
└── client.cpp   # TCP client — connects to server, sends/receives on separate threads
`
