#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <string>

using namespace std;

string readLine(int sock) {
    string result;
    char c;
    while (recv(sock, &c, 1, 0) == 1 && c != '\n')
        result += c;
    return result;
}

void relay(int from, int to, const string& username) {
    char buffer[1024];
    while (true) {
        ssize_t n = recv(from, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            cout << username << " disconnected\n";
            string msg = username + " disconnected.\n";
            send(to, msg.c_str(), msg.size(), 0);
            break;
        }
        buffer[n] = '\0';
        string message = username + ": " + buffer;
        send(to, message.c_str(), message.size(), 0);
    }
}

int main() {
    int serverFD = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFD == -1) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverFD, (sockaddr*)&addr, sizeof(addr)) == -1) { perror("bind"); return 1; }
    if (listen(serverFD, 2) == -1) { perror("listen"); return 1; }

    cout << "Waiting for 2 clients on port 8080...\n";

    int client1 = accept(serverFD, nullptr, nullptr);
    string name1 = readLine(client1);
    cout << name1 << " connected\n";
    string waiting = "Waiting for second client...\n";
    send(client1, waiting.c_str(), waiting.size(), 0);

    int client2 = accept(serverFD, nullptr, nullptr);
    string name2 = readLine(client2);
    cout << name2 << " connected. Chat started.\n";
    string ready1 = name2 + " joined. Start chatting!\n";
    string ready2 = name1 + " is here. Start chatting!\n";
    send(client1, ready1.c_str(), ready1.size(), 0);
    send(client2, ready2.c_str(), ready2.size(), 0);

    thread t1(relay, client1, client2, name1);
    thread t2(relay, client2, client1, name2);

    t1.join();
    t2.join();

    close(client1);
    close(client2);
    close(serverFD);
    return 0;
}
