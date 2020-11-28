CXX := g++
CXXFLAGS := -g -Wall -O2 -DSUPPORT_ICONV_ENCODER=1 -DSUPPORT_MYSQL_DB=1
TARGET := libcpplib.a
INCLUDES := -I../
LIB32_PATHS := 
LIBS :=
OBJSDIR := obj
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp,$(OBJSDIR)/%.o,$(SRCS))
#子目录
APPLICATION_DIR := ../application
CHECKSUM_DIR := ../checksum
MUTEX_DIR := ../mutex
PROTOC_DIR := ../protocolbuf
STREAM_DIR := ../stream
THREAD_DIR := ../thread
BASE_DIR := ../base
CHARSET_DIR := ../charset
DB_DIR := ../database
MIMEPP_DIR := ../mimepp/src
NET_DIR := ../network
SOCK_DIR := ../socketclient
SYSTEM_DIR := ../system
THIRDLIB_DIR := ../thirdparty
WEB_DIR := ../web

#sb base目录的string.h会覆盖标准的
#INCLUDES += -I$(APPLICATION_DIR) -I$(CHECKSUM_DIR) -I$(MUTEX_DIR) -I$(PROTOC_DIR) -I$(STREAM_DIR) -I$(THREAD_DIR) -I$(BASE_DIR) -I$(CHARSET_DIR) -I$(DB_DIR) -I$(MIMEPP_DIR) -I$(NET_DIR) -I$(SOCK_DIR) -I$(SYSTEM_DIR) -I$(WEB_DIR) -I$(THIRDLIB_DIR)/include

INCLUDES += -I$(MIMEPP_DIR) -I$(THIRDLIB_DIR)/include
LIB32_PATHS += -L$(THIRDLIB_DIR)/lib32
LIBS += -liconv -lmysqlclient -lpthread -lresolv 

SRCS += $(wildcard $(APPLICATION_DIR)/*.cpp)

SRCS += $(wildcard $(CHECKSUM_DIR)/*.cpp)

SRCS += $(wildcard $(MUTEX_DIR)/*.cpp)

SRCS += $(wildcard $(PROTOC_DIR)/*.cpp)

SRCS += $(wildcard $(STREAM_DIR)/*.cpp)

SRCS += $(wildcard $(THREAD_DIR)/*.cpp)

SRCS += $(wildcard $(BASE_DIR)/*.cpp)

SRCS += $(wildcard $(CHARSET_DIR)/*.cpp)

SRCS += $(wildcard $(DB_DIR)/*.cpp)

SRCS += $(wildcard $(MIMEPP_DIR)/*.cpp)

SRCS += $(wildcard $(NET_DIR)/*.cpp)

SRCS += $(wildcard $(SOCK_DIR)/*.cpp)

SRCS += $(wildcard $(SYSTEM_DIR)/*.cpp)

SRCS += $(wildcard $(WEB_DIR)/*.cpp)
#
OBJS += $(patsubst %.cpp,%.o,$(SRCS))


$(shell mkdir -p $(OBJSDIR))

all:$(TARGET)

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET):$(OBJS)
	ar rc $@ $^ 
	-ranlib $@
	cp $@ ../lib 

clean:
	rm -rf $(TARGET) $(APPLICATION_DIR)/*.o $(CHECKSUM_DIR)/*.o $(MUTEX_DIR)/*.o $(PROTOC_DIR)/*.o $(STREAM_DIR)/*.o $(THREAD_DIR)/*.o $(BASE_DIR)/*.o $(CHARSET_DIR)/*.o $(DB_DIR)/*.o $(MIMEPP_DIR)/*.o $(NET_DIR)/*.o $(SOCK_DIR)/*.o $(SYSTEM_DIR)/*.o $(WEB_DIR)/*.o
