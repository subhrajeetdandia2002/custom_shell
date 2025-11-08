CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET = myshell
SRC = myshell.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
