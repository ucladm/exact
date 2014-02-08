CC = g++

IDIR = include
ODIR = src

COPTS =	-fPIC -DLINUX -Wall \
        $(shell root-config --cflags) 

LIBS = $(shell root-config --glibs)

CFLAGS = -c -g -Wall -I$(IDIR)
VPATH = src
LDFLAGS= -g

DEPS = $(wildcard $(IDIR)/*.hh)
#_OBJS = $(DEPS:.hh=.o)
_SRCS = $(wildcard $(ODIR)/*.cc)
_OBJS = $(_SRCS:.cc=.o)
OBJS = $(patsubst $(IDIR)/%,$(ODIR)/%,$(_OBJS))


DICT = libDict
DICTSUF = cxx
DICTSRC = $(DICT).$(DICTSUF)

EXEC1 = tpcanalysis
EXEC2 = eventviewer


#############################################################

all: $(OBJS) $(DICTSRC) $(EXEC1) $(EXEC2)

# compile each class individually; recompile only those with changes
$(ODIR)/%.o: %.cc $(DEPS)
	@echo [${CC}] Compiling class $<
	@$(CC) $(CFLAGS) $(COPTS) $< -o $@

# generate dictionary for all classes, including shared dictionary 
# that gets loaded into interactive ROOT
$(DICTSRC): $(DEPS) $(OBJS)
	@echo [ROOTCINT] Generating dictionary $(DICTSRC)
	@rootcint -l -f $(DICTSRC) -c $(DEPS) LinkDef.h
	@echo [${CC}] Compiling dictionary $(DICTSRC)
	@$(CC) $(CFLAGS) $(COPTS) -c $(DICTSRC) -o $(DICT).o
	@echo [${CC}] Generating shared library $(DICT).so
	@$(CC) -shared  -o $(DICT).so  $(OBJS) $(DICT).o $(LIBS)

# compile executable
$(EXEC1).o : $(EXEC1).cc
	@echo [${CC}] Compiling $(EXEC1).cc
	@$(CC) $(CFLAGS) $(COPTS) $< -o $@

$(EXEC2).o : $(EXEC2).cc
	@echo [${CC}] Compiling $(EXEC2).cc
	@$(CC) $(CFLAGS) $(COPTS) $< -o $@

# link everything
$(EXEC1): $(OBJS) $(EXEC1).o
	@echo [${CC}] Linking $(EXEC1)
	@$(CC) $(EXEC1).o $(OBJS) $(DICT).o -o $@ $(LDFLAGS) $(LIBS)
	@echo [DONE]

$(EXEC2): $(OBJS) $(EXEC2).o
	@echo [${CC}] Linking $(EXEC2)
	@$(CC) $(EXEC2).o $(OBJS) $(DICT).o -o $@ $(LDFLAGS) $(LIBS)
	@echo [DONE]


.PHONY: clean

clean:
	rm -rf $(ODIR)/*.o $(DICT).* *.o $(EXEC1) $(EXEC2)