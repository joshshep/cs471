CC = g++
CPPFLAGS = -Wall -std=c++11 -O3 #-g -DVERBOSE -DDEBUG # -fopenmp -lpthread

# g++ -fopenmp -lpthread -std=c++11 -g pagerank.cpp -o test

#CFLAGS=-std=c99 -Wall -O0 -m32# -DVERBOSE# -Iinclude

#CFLAGS += #-Wno-unused-variable
#VPATH = src include
#CPPFLAGS=-std=c++11 -O3 -Wall
#LDFLAGS=
SRC_DIR := .
OBJ_DIR := .
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

#INCLUDE=
OBJ = $(SRC:.cpp=.o)
TARGET = sol

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(LDFLAGS) $(CPPFLAGS) -o $(TARGET) $(OBJ_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $<

run: all
	@./$(TARGET)

clean:
	rm -rf $(OBJ_FILES) $(TARGET)
