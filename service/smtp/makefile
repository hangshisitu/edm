TARGET  := smtpsvr
RFCPATH := ../cpplib
SUBDIRS := .
SOURCES := $(foreach subdir, $(SUBDIRS), $(wildcard $(subdir)/*.cpp))
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))
DEPENDS := $(OBJECTS:.o=.d)
INCDIR  := $(RFCPATH) $(RFCPATH)/thirdparty/include $(RFCPATH)/mimepp/src
THIRDLIB_DIR := $(RFCPATH)/thirdparty
LIBDIR  := $(RFCPATH)/lib $(THIRDLIB_DIR)/lib64
LIBS    := pthread  resolv db cpplib mysqlclient 
CC      := g++
CFLAGS  := -pipe -Wpointer-arith -D_SMTP_CACHE -D_REENTRANT -D_GNU_SOURCE  -D__daemon__

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
