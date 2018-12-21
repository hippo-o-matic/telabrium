CXX = g++
CPPFLAGS = -Wall -g -std=c++17 -Wl,-rpath=$(LIB_PATH)

INCLUDE_PATH = include
LIB_PATH = lib

SRC_PATH = src
BUILD_PATH = build
TARGET = hippotest

SRC = $(wildcard $(SRC_PATH)/*.*)

OBJ = $(SRC)
OBJ := $(patsubst $(SRC_PATH)/%.cpp,$(BUILD_PATH)/%.cpp.o,$(OBJ))
OBJ := $(patsubst $(SRC_PATH)/%.c,$(BUILD_PATH)/%.c.o,$(OBJ))

#Ensure glfw is after GL
LIBS = -lGL -lglfw -lassimp -lX11 -lpthread -lXrandr -lXi -ldl -llua5.3 -larchive 


all: $(BUILD_PATH)/bin/$(TARGET)

$(BUILD_PATH)/bin/$(TARGET): $(OBJ)
	@-$(MKDIR_P) $(dir $@)
	@-echo "Linking..."
	@-$(CXX) $(CPPFLAGS) $^ -o $@ -I$(INCLUDE_PATH) -L$(LIB_PATH) $(LIBS)
	@-echo "\n****************************************\nSucessfully built $(TARGET)\n****************************************"

$(BUILD_PATH)/%.cpp.o: $(SRC_PATH)/%.cpp
	$(CXX) $(CPPFLAGS) -c $< -I$(INCLUDE_PATH) -o $@

$(BUILD_PATH)/%.c.o: $(SRC_PATH)/%.c
	$(CXX) $(CPPFLAGS) -c $< -I$(INCLUDE_PATH) -o $@
	

clean: 
	rm -rf $(BUILD_PATH)/bin/$(TARGET) 
	rm -rf $(BUILD_PATH)/%.o

MKDIR_P ?= mkdir -p