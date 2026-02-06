CXX = g++
CXXFLAGS = -std=c++14 -I.. -I../src
TARGET = test
BUILD_DIR = build

run: build
	./$(BUILD_DIR)/$(TARGET)

build: all

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): test.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -rf $(BUILD_DIR)