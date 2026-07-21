CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./DecisionTree/include
SFML_FLAGS = -lsfml-audio -lsfml-system

SRCS = reconhecimento.cpp DecisionTree/src/DecisionTree.cpp DecisionTree/src/Metrics.cpp Preprocessing/dtw_apply.cpp Preprocessing/wavelet_apply.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = reconhecimento

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(SFML_FLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)