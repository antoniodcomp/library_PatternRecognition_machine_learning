CXX = g++
CXXFLAGS = -std=c++14 -Wall -I./DecisionTree/include

SRCS = main.cpp DecisionTree/src/DecisionTree.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
