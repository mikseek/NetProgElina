CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

TARGETS = echo_client echo_server

all: $(TARGETS)

echo_client: echo_client.cpp
	$(CXX) $(CXXFLAGS) -o echo_client echo_client.cpp

echo_server: echo_server.cpp
	$(CXX) $(CXXFLAGS) -o echo_server echo_server.cpp

clean:
	rm -f $(TARGETS)

.PHONY: all clean
