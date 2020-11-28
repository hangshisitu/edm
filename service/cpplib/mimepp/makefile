CXX := g++
CXXFLAGS := -g -Wall
TARGET := libmimpp.a
INCLUDES := -Isrc
LIBS :=
OBJSDIR := obj
SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp,$(OBJSDIR)/%.o,$(SRCS))

$(shell mkdir -p $(OBJSDIR))

all:$(TARGET)

$(OBJSDIR)/%.o:src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET):$(OBJS)
	ar rc $@ $^ 
	-ranlib $@ 
clean:
	rm -rf $(TARGET) $(OBJSDIR)/*.o
