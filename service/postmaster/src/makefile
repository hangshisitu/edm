TARGET  := ../postmaster
SUBDIRS := .
SOURCES := $(foreach subdir, $(SUBDIRS), $(wildcard $(subdir)/*.cpp))
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))
DEPENDS := $(OBJECTS:.o=.d)
INCDIR  := 
LIBDIR  := 
LIBS    := pthread
STATIC  := 
CC      := g++
CFLAGS  := -g -O2 -Wall -pipe -W -Wpointer-arith -D_REENTRANT -D_GNU_SOURCE

$(TARGET) : $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS) $(STATIC) $(addprefix -L,$(LIBDIR)) $(addprefix -l,$(LIBS))
	chmod +x $(TARGET)

rb : cl $(TARGET)

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

.PHONY:cl

clean:
	-rm -f $(DEPENDS)
	-rm -f ${OBJECTS}
	-rm -f $(TARGET)
