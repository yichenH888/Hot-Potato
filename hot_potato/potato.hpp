#include <cstdio>
#include <cstdlib>
#include <vector>

using namespace std;

class Potato {
   public:
    int hopNum;
    int hopCounter;
    int pathID[BUFFERSIZE];
    Potato() : hopNum(0), hopCounter(0) {
        // fill(pathID,pathID+BUFFERSIZE,-1);
        for (int i = 0; i < BUFFERSIZE; i++) {
            pathID[i] = -1;
        }
    }
    Potato(int num) : hopNum(num), hopCounter(num) {}
    ~Potato() {}
};