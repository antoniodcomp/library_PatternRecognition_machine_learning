CXX = g++
CXXFLAGS = -std=c++14 -Wall -I./DecisionTree/include
SFML_FLAGS = -lsfml-audio -lsfml-system

SRCS = main.cpp DecisionTree/src/DecisionTree.cpp DecisionTree/src/Metrics.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(SFML_FLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)