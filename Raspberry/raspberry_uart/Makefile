# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall

# Libraries
LIBS = -ludev -levdev -lpthread

# Source files
SRCS = main.cpp controller.cpp overlay.cpp lodepng.cpp monitor.cpp pico_monitor.cpp gpio_monitor.cpp properties.cpp GPIO.cpp

# Header files
HDRS = controller.h overlay.h font5x7.h lodepng.h shared_memory.h monitor.h pico_monitor.h gpio_monitor.h properties.h GPIO.h

# Output executable
TARGET = joystick_emulator

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link the target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Compile source files
%.o: %.cpp $(HDRS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
