#include "helper.hpp"
#include "potato.hpp"
#include "singlePlayer.hpp"
#include "string.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        errMsg("Wrong number of input!");
    }
    // Set values from input
    const char *hostname_master = argv[1];
    const char *port_master = argv[2];

    PlayerFromPlayer onePlayer;
    onePlayer.socket_fd = buildClientSocket(port_master, hostname_master);  // communicate with ringmaster

    recv(onePlayer.socket_fd, &onePlayer.id, sizeof(onePlayer.id), 0);
    recv(onePlayer.socket_fd, &onePlayer.numPlayer, sizeof(onePlayer.numPlayer), 0);

    int player_socket = buildServerSocket("");

    struct sockaddr_in sock;
    socklen_t len = sizeof(sock);
    getsockname(player_socket, (struct sockaddr *)&sock, &len);

    int player_port = ntohs(sock.sin_port);
    send(onePlayer.socket_fd, &player_port, sizeof(player_port), 0);

    cout << "Connected as player " << onePlayer.id << " out of " << onePlayer.numPlayer << " total players" << endl;
    
    //  build communication with left neighbour, work as server
    int rightNei_port = 0;
    recv(onePlayer.socket_fd, &rightNei_port, sizeof(rightNei_port), MSG_WAITALL);
    char rightNeigh_port_str[BUFFERSIZE];
    sprintf(rightNeigh_port_str, "%d", rightNei_port);
    const char *rightNeigh_port_cstr = rightNeigh_port_str;

    char rightNei_ip[BUFFERSIZE];
    recv(onePlayer.socket_fd, &rightNei_ip, sizeof(rightNei_ip), MSG_WAITALL);
    onePlayer.rightNei_socket = buildClientSocket(rightNeigh_port_cstr, rightNei_ip);

    //  build communicatio with left neighbour, work as server
    onePlayer.leftNei_socket = acceptServer(&onePlayer.ip, player_socket);

    // Start to receive potato
    Potato potato;

    int allFD[3] = {0};              // store all the three FD
    allFD[0] = onePlayer.socket_fd;  // ringmaster
    allFD[1] = onePlayer.rightNei_socket;
    allFD[2] = onePlayer.leftNei_socket;

    int maxFD = *max_element(allFD, allFD + 3);

    fd_set readfds;
    srand((unsigned int)time(NULL) + onePlayer.id);

    while (1) {  // receive potato and send to the other player until hopCounter = -1 potato is received
        FD_ZERO(&readfds);
        for (int i = 0; i < 3; i++) {
            if (allFD[i] >= 0) {
                FD_SET(allFD[i], &readfds);
            }
        }

        int retSelect = select(maxFD + 1, &readfds, NULL, NULL, NULL);
        if (retSelect == 0) {
            cout << "Error (Select): Timeout expired!" << endl;
        } else if (retSelect == -1) {
            perror("Error (select) !");
            exit(EXIT_FAILURE);
        } else {
            for (int i = 0; i < 3; i++) {
                if (FD_ISSET(allFD[i], &readfds)) {
                    // Read or write data from the ready file descriptor as needed
                    recv(allFD[i], &potato, sizeof(potato), MSG_WAITALL);
                    int pathIndex = potato.hopNum - potato.hopCounter - 1;
                    potato.pathID[pathIndex] = onePlayer.id;
                    break;
                }
            }
        }

        if (potato.hopCounter == -1) {  // shut down message
            break;
        }

        if (potato.hopCounter == 0) {  // send the potato to ringmaster
            potato.hopCounter--;
            send(onePlayer.socket_fd, &potato, sizeof(potato), 0);
            cout << "I'm it" << endl;
        } else {  // send the potato to a random player
            int randomPlayer = rand() % 2;
            if (randomPlayer == 0) {  // send to right neighbour
                potato.hopCounter--;
                send(onePlayer.rightNei_socket, &potato, sizeof(potato), 0);
                int rightNei_id = (onePlayer.id + 1) % onePlayer.numPlayer;
                cout << "Sending potato to " << rightNei_id << endl;
            } else {  // send to left neighbour
                potato.hopCounter--;
                send(onePlayer.leftNei_socket, &potato, sizeof(potato), 0);
                int leftNei_id = (onePlayer.id - 1 + onePlayer.numPlayer) % onePlayer.numPlayer;
                cout << "Sending potato to " << leftNei_id << endl;
            }
        }
    }

    for (int i = 0; i < 3; i++) {
        close(allFD[i]);
    }

    return 0;
}