#include <arpa/inet.h>
#include <atomic>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace std;

atomic<bool> running(true);

void receive(int sock) {
  char buffer[1024];
  while (running) {
    ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
      cout << "\nDisconnected from server.\n";
      running = false;
      break;
    }
    buffer[n] = '\0';
    cout << buffer << flush;
  }
}

int main(int argc, char* argv[]) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    perror("socket");
    return 1;
  }

  const char* serverIP = (argc > 1) ? argv[1] : "127.0.0.1";

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8080);
  inet_pton(AF_INET, serverIP, &addr.sin_addr);

  if (connect(sock, (sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("connect");
    return 1;
  }

  string username;
  cout << "Enter your username: ";
  getline(cin, username);
  string nameMsg = username + '\n';
  send(sock, nameMsg.c_str(), nameMsg.size(), 0);

  cout << "Connected. Type messages and press Enter. Type 'quit' to exit.\n";

  thread recvThread(receive, sock);

  string message;
  while (running && getline(cin, message)) {
    if (message == "quit")
      break;
    message += '\n';
    send(sock, message.c_str(), message.size(), 0);
  }

  running = false;
  close(sock);
  recvThread.join();
  return 0;
}
