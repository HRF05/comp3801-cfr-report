CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET = RPSRegretMatching

all: $(TARGET)

$(TARGET): rps.cpp rps.h
	$(CXX) $(CXXFLAGS) rps.cpp -o $(TARGET)

clean:
	rm -f rps.o $(TARGET)

.PHONY: all clean