EXE = readable 
SHELL := /bin/bash
CC = gcc
OBJDIR = obj
SRC = $(wildcard *.c)
HDR = $(wildcard *.h)
OBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(SRC))
LIBS = -lm -std=c99  -Wall

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $(EXE) $(LIBS)

$(OBJDIR)/%.o: %.c $(HDR) | $(OBJDIR)
	$(CC) -c $< -o $@ $(LIBS)

$(OBJDIR):
	mkdir -p $@


all: $(SRC) $(HDR)
	$(CC) $(SRC) -o $(EXE) $(LIBS)
 
clean:
	rm -rf $(OBJDIR) $(EXE)

debug: $(SRC) $(HDR)
	$(CC) $(SRC) -g -o $(EXE) $(LIBS)

run:
	make
	./readable
	

