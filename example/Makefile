CXX = g++
CXXFLAGS = -std=c++14 -I. -I../src -D__MACOSX_CORE__
LDFLAGS = -framework CoreAudio -framework CoreFoundation -lpthread
TARGET = build/portaklon_audio
BUILD_DIR = build

all: $(TARGET)

$(TARGET): $(BUILD_DIR)/main.o $(BUILD_DIR)/RtAudio.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/main.o: main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/RtAudio.o: rtaudio/RtAudio.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean