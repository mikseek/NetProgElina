CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = daytime_client

all: $(TARGET)

$(TARGET): daytime_client.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) daytime_client.cpp

clean:
	rm -f $(TARGET)
