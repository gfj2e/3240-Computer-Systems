# Compiler and fla# Compiler and flags
CXX = g++
CXXFLAGS = -g -O1 -Wall -std=c++17
LDLIBS = -lpthread

# Source files
CLIENT_SRC = client.cpp
SERVER_SRC = server.cpp Board.cpp Tile.cpp

# Object files
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

# Targets
all: client server

client: $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)
	@rm -f $(CLIENT_OBJ)

server: $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)
	@rm -f $(SERVER_OBJ)

clean:
	rm -f *.o *~ *.exe client server
