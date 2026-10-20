CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = echo_client

all: $(TARGET)

$(TARGET): echo_client.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) echo_client.cpp

clean:
	rm -f $(TARGET)
