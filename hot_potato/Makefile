TARGETS=player ringmaster
CFLAGS = -Werror -Wall -pedantic -std=c++11

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

player: player.cpp helper.hpp helper.cpp potato.hpp singlePlayer.hpp
	g++ -g $(CFLAGS) -o player player.cpp helper.cpp

ringmaster: ringmaster.cpp helper.hpp helper.cpp potato.hpp singlePlayer.hpp
	g++ -g $(CFLAGS) -o ringmaster ringmaster.cpp helper.cpp 