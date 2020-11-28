CXX := g++
CXXFLAGS := -g -Wall -O2 
TARGET := 
INCLUDES := -I.. 
LIB32_PATHS := 
LIBS :=
OBJSDIR := obj
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,$(OBJSDIR)/%.o,$(SRCS))
#子目录
CPPLIB_PATH :=../../cpplib
MIMEPP_DIR := $(CPPLIB_PATH)/mimipp
THIRDLIB_DIR := $(CPPLIB_PATH)/thirdparty

INCLUDES += -I$(CPPLIB_PATH)  -I$(MIMEPP_DIR)/src -I$(THIRDLIB_DIR)/include
LIB32_PATHS += -L$(THIRDLIB_DIR)/lib32 -L$(CPPLIB_PATH)/lib -L/usr/lib64/mysql
LIBS += -lpthread -lresolv -lcpplib
#
OBJS += $(patsubst %.cpp,%.o,$(SRCS))

#$(shell mkdir -p $(OBJSDIR))

TARGET := deliverengine
#ifeq ($(shell getconf LONG_BIT), 32) 
#    TARGET := $(TARGET).x86
#else
#    TARGET := $(TARGET).x64
#endif

TARGET_OBJS = deliverengine.o mxmgr.o requestmail.o emlsender.o edcode.o
CLEAN_OBJS = mailclean.o mxmgr.o  sender.o 

all : $(TARGET) mailclean

%.o:../%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< 

$(TARGET):$(TARGET_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIB32_PATHS) $(LIBS)
	cp $@ ..

mailclean:$(CLEAN_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIB32_PATHS) $(LIBS)
	cp $@ ..	
clean:
	rm -rf $(TARGET) mailclean ./*.o 
