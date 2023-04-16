#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <algorithm>

#define BUFFERSIZE 512
using namespace std;
void errMsg(const char * msg);
int buildServerSocket(const char * port);
int acceptServer(string * ip, int serverFD);
int buildClientSocket(const char * port, const char * hostname);

 