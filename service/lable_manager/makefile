TARGET  := labelmgr
SUBDIRS := .
SOURCES := $(foreach subdir, $(SUBDIRS), $(wildcard $(subdir)/*.cpp))
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))
DEPENDS := $(OBJECTS:.o=.d)
CPPLIB  := ../cpplib/thirdparty
INCDIR  := $(CPPLIB)/include
LIBDIR  := $(CPPLIB_PATH)/thirdparty/lib64 
LIBS    := pthread mysqlclient rt
CC      := g++
CFLAGS  := -Wall -pipe -W -Wpointer-arith -D_REENTRANT -D_GNU_SOURCE

all : $(TARGET)

rb : clean all

$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS))
	chmod +x $(TARGET)

ifeq (,$(MAKECMDGOALS))
    sinclude $(DEPENDS)
else
    ifneq ($(%clean,,$(MAKECMDGOALS)),)
        sinclude $(DEPENDS)
    endif
endif

%.d:%.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $(addprefix -I,$(INCDIR)) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;\
	rm -f $@.$$$$

%.o:%.cpp
	$(CC) -c ${CFLAGS} $(addprefix -I,$(INCDIR)) $< -o $@

.PHONY:clean

clean:
	-rm -f $(DEPENDS)
	-rm -f ${OBJECTS}
	-rm -f $(TARGET)
