OUTNAME := ./bin/deliverhelper
TARGET  := $(OUTNAME)
SUBDIRS := .
SOURCES := $(foreach subdir, $(SUBDIRS), $(wildcard $(subdir)/*.cpp))
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))
DEPENDS := $(OBJECTS:.o=.d)
CPPLIB_PATH := ../cpplib
INCDIR  := $(CPPLIB_PATH)/thirdparty/include 
LIBDIR  := $(CPPLIB_PATH)/thirdparty/lib64
LIBS    := pthread mysqlclient rt db
CC      := g++
CFLAGS  := -g -Wall -pipe -W -Wpointer-arith -D_REENTRANT -D_GNU_SOURCE

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS))
	chmod +x $(TARGET)

all : load rb
rb : clean $(TARGET)

ifeq (,$(MAKECMDGOALS))
    sinclude $(DEPENDS)
else
    ifneq ($(%clean,,$(MAKECMDGOALS)),)
        sinclude $(DEPENDS)
    endif
endif

ifeq ($(shell getconf LONG_BIT), 32)
    TARGET := $(OUTNAME).x86
else
    TARGET := $(OUTNAME).x64
endif

%.d:%.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $(addprefix -I,$(INCDIR)) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;\
	rm -f $@.$$$$

%.o:%.cpp
	$(CC) -c ${CFLAGS} $(addprefix -I,$(INCDIR)) $< -o $@

.PHONY:clean

load:
	svn update

clean:
	-rm -f $(DEPENDS)
	-rm -f ${OBJECTS}
	-rm -f $(TARGET)
