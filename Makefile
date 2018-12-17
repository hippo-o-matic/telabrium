CXX = clang++
CFLAGS = -Wall -g -std=c++17 -Wl,-rpath=$(LIB_PATH)

INCLUDE_PATH = include
LIB_PATH = lib

SRC_PATH = src
BUILD_PATH = build
TARGET = hippotest

SRC = $(wildcard $(SRC_PATH)/*/*.* $(SRC_PATH)/*.*) 
OBJ = $(SRC: .c=.o, .cpp=.o)
#Ensure glfw is after GL
LIBS = -lGL -lglfw -lassimp -lX11 -lpthread -lXrandr -lXi -ldl -llua5.3 -larchive 


all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CFLAGS) $? -o $(BUILD_PATH)/bin/$@ -I$(INCLUDE_PATH) -L$(LIB_PATH) $(LIBS)

%.o: %.cpp %.c
	$(CXX) $(CFLAGS) -c $? -o $(BUILD_PATH)/$(OBJ) -I$(INCLUDE_PATH)

clean: 
	rm -rf $(BUILD_PATH)/bin/$(TARGET) 
	rm -rf $(BUILD_PATH)/%.o