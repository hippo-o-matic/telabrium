CXX = g++
CFLAGS = -Wall -g -std=c++17 -Wl,-rpath=$(LIB_PATH)
LDFLAGS =
TARGET = hippotest

INCLUDE_PATH = -Iinclude/
LIB_PATH = -Llib/
SRC_PATH = src

SRC = $(wildcard $(SRC_PATH)/*.cpp $(SRC_PATH)/devices/*.cpp $(SRC_PATH)/*.c)
OBJS = $(SRC: .cpp = .o, .c = .o)
#Ensure glfw is after GL
LIBS = -lGL -lglfw -lassimp -lX11 -lpthread -lXrandr -lXi -ldl -llua5.3


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) $? $(INCLUDE_PATH) $(LIB_PATH) $(LIBS) -o $@

.cpp.o: 
	$(CXX) $(CFLAGS) $(INCLUDE_PATH) -c $<

clean: 
	rm -rf $(TARGET) *.o