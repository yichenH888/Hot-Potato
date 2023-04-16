#include "helper.hpp"
#include "potato.hpp"
#include "singlePlayer.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        errMsg("Wrong number of input!");
    }
    // Set values from input
    const char *port_num = argv[1];
    int num_players = atoi(argv[2]);
    int num_hops = atoi(argv[3]);

    if (num_players < 2 || num_hops < 0 || num_hops > 512) {//corner case
        cerr << "invalid input" << endl;
        exit(EXIT_FAILURE);
    }
    
    cout << "Potato Ringmaster" << endl;
    cout << "Players = " << num_players << endl;
    cout << "Hops = " << num_hops << endl;

    // Establish server socket
    int serverFD = buildServerSocket(port_num);
    vector<PlayerFromMaster> players;
    
    // Establish connection with players
    for (int i = 0; i < num_players; i++) {
        PlayerFromMaster onePlayer; 

        int clientFD = acceptServer(&onePlayer.ip, serverFD);
        // assign corresponding information
        onePlayer.numPlayer = num_players;
        onePlayer.socket_fd = clientFD;
        onePlayer.id = i;

        // provide relevant information to each player
        send(clientFD, &i, sizeof(i), 0);                      // ID
        send(clientFD, &num_players, sizeof(num_players), 0);  // num_players

        recv(clientFD, &onePlayer.port, sizeof(onePlayer.port), 0);

        players.push_back(onePlayer);

        cout << "Player " << onePlayer.id << " is ready to play" << endl;
    }

    // send neighbour information (ip and port)
    for (int i = 0; i < num_players; i++) {  
        int rightNeigh_id = (i + 1) % num_players;

        char rightNeigh_ip[BUFFERSIZE];  // BUFFERSIZE enough or not???
        memset(rightNeigh_ip, 0, sizeof(rightNeigh_ip));
        strcpy(rightNeigh_ip, players[rightNeigh_id].ip.c_str());

        send(players[i].socket_fd, &players[rightNeigh_id].port, sizeof(players[rightNeigh_id].port), 0);
        send(players[i].socket_fd, rightNeigh_ip, BUFFERSIZE, 0);
    }

    // create potato object
    Potato potato(num_hops);

    // randomly choose a player//why use num_players??
    srand((unsigned int)time(NULL) + num_players);
    int randomPlayer = rand() % num_players;

    // send the "potato" to the selected player (first sending)
    potato.hopCounter--;
    for (int i = 0; i < BUFFERSIZE; i++) {
        potato.pathID[i] = -1;
    }
    
    if (num_hops > 0) {
        send(players[randomPlayer].socket_fd, &potato, sizeof(potato), 0);
        cout << "Ready to start the game, sending potato to player " << randomPlayer << endl;

        // receive potato with hopCOunter == 1, indicate the end of the game
        int *allFD = new int[num_players];
        for (int i = 0; i < num_players; i++) {
            allFD[i] = players[i].socket_fd;
        }
        int maxFD = *max_element(allFD, allFD + num_players);

        fd_set readfds;

        FD_ZERO(&readfds);
        for (int i = 0; i < num_players; i++) {
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
            for (int i = 0; i < num_players; i++) {
                if (FD_ISSET(allFD[i], &readfds)) {
                    //  Read or write data from the ready file descriptor as needed
                    recv(allFD[i], &potato, sizeof(potato), MSG_WAITALL);
                    break;
                }
            }
        }

        // print trace
        cout << "Trace of potato:" << endl;
        int i = 0;
        while (potato.pathID[i] != -1) {
            cout << potato.pathID[i];
            if (potato.pathID[i + 1] != -1) {
                cout << ",";
            }
            i++;
        }
        cout << endl;

        delete[] allFD;
    }

    // shut the game down: send potato with 
    for (int i = 0; i < num_players; i++) {
        send(players[i].socket_fd, &potato, sizeof(potato), 0);
    }

    //close sockets
    close(serverFD);
    for (int i = 0; i < num_players; i++) {
        close(players[i].socket_fd);
    }

    return 0;
}