CX = g++
CXXFLAGS = -g -Wall -I/usr/local/include/dynamixel_sdk `pkg-config opencv4 --cflags`
LDFLAGS = `pkg-config opencv4 --libs`
TARGET = linedetect
OBJS = main.o vision.o

$(TARGET): $(OBJS)
	$(CX) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.cpp
	$(CX) $(CXXFLAGS) -c main.cpp

vision.o: vision.hpp vision.cpp
	$(CX) $(CXXFLAGS) -c vision.cpp

.PHONY: clean
clean:
	rm -rf $(TARGET) $(OBJS)
