CC = g++ -W
CXX = g++ -W
AR = ar rc
RANLIB = ranlib

DEBUG_FLAG=-DNDEBUG

CPPLIB_PATH = ../../cpplib
SYSTEM_INCLUDE_FLAGS=-I$(CPPLIB_PATH)/thirdparty/include -I$(CPPLIB_PATH)/thirdparty/include/db4 -I$(CPPLIB_PATH)/mimepp/src -I$(CPPLIB_PATH)
SYSTEM_LINK_FLAGS=-L./ -L$(CPPLIB_PATH)/thirdparty/lib64

ICONV_INCLUDE_FLAGS=#-DSUPPORT_ICONV_ENCODER=1
ICONV_LINK_FLAGS=#-liconv

MYSQL_INCLUDE_FLAGS=-DSUPPORT_MYSQL_DB=1
MYSQL_LINK_FLAGS=-lmysqlclient

ORACLE_INCLUDE_FLAGS=#-DSUPPORT_ORACLE_DB=1
ORACLE_LINK_FLAGS=#-locci -lclntsh

MIMEPP_LINK_FLAGS = 

CPPLIB_LINK_FLAGS = -L$(CPPLIB_PATH)/lib -lcpplib

CXX_FLAGS = -g -Wall $(CXX_OPT) $(SYSTEM_INCLUDE_FLAGS) $(ICONV_INCLUDE_FLAGS) $(MYSQL_INCLUDE_FLAGS) $(ORACLE_INCLUDE_FLAGS) -I.. 
LINK_FLAGS = $(CPPLIB_LINK_FLAGS) $(MIMEPP_LINK_FLAGS) $(SYSTEM_LINK_FLAGS) $(ICONV_LINK_FLAGS) $(MYSQL_LINK_FLAGS) $(ORACLE_LINK_FLAGS) -lpthread -lresolv -ldb 
VPATH = ../

.SUFFIXES: .cpp .c .no
.cpp.o:
	$(CXX) -c $(CXX_FLAGS) $(MAKEDEF_CXX_FLAGS) $<
.c.o:
	$(CC) -c $(CXX_FLAGS) $(MAKEDEF_CXX_FLAGS) $<

ALLDEPOBJS = berkeleydb.o lock.o cachedb.o  ip_area.o parserequest.o  
OUTPUT = cachedb 

all : $(OUTPUT)

debug:
	make DEBUG_FLAG=-DDEBUG all

release: 
	make DEBUG_FLAG=-DNDEBUG all

r: clean all

clean:
	rm -f *.so *.o *.d core.* $(OUTPUT)

cachedb : $(ALLDEPOBJS)
	$(CXX) $(CXX_FLAGS) -o cachedb $(ALLDEPOBJS) $(LINK_FLAGS)
	cp $@ ..


####################################### Commands to generate dependency files #######################################
GEN_DEPS_C= $(CC) -E -MM $(DEFS) $(CXX_FLAGS) 
GEN_DEPS_CXX= $(CXX) -E -MM $(DEFS) $(CXX_FLAGS) 
%.d : %.c 
	@echo "generate dependency file for $<" 
	@$(SHELL) -ec '$(GEN_DEPS_C) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

%.d : %.cpp
	@echo "generate dependency file for $<"
	@$(SHELL) -ec '$(GEN_DEPS_CXX) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@' 

OBJSDEP = $(ALLDEPOBJS:.o=.d)

ifeq (,$(MAKECMDGOALS)) 
-include $(OBJSDEP)
else
ifneq ($(%clean,,$(MAKECMDGOALS)),)
-include $(OBJSDEP) 
endif
endif
