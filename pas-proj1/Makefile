#
# ppc3 makefile
#
#

#
# use which C compiler?
#
CC = gcc

#
# use which parser generator?
#
YACC = bison

#
# Yacc/Bison flags
#
YFLAGS = -d -y

#
# use which scanner generator?
#
LEX = flex

#
# compile everything for debugger?
#
#yes
CFLAGS = -g
#no
#CFLAGS = 

# Currently not used
#
# PPC3SRC = main.c message.c symtab.c types.c utils.c backend.c tree.c encode.c

PPC3H	= defs.h types.h symtab.h tree.h backend.h 

PPC3OBJ = main.o message.o symtab.o types.o utils.o gram.o scan.o backend.o tree.o encode.o

# ppc3 rules
#
ppc3     : $(PPC3OBJ)
	$(CC) $(CFLAGS) $(PPC3OBJ) -o ppc3

# dependencies for compiler modules

main.o: main.c defs.h types.h symtab.h

types.o: types.c types.h symtab.h message.h

symtab.o: symtab.c types.h symtab.h message.h

backend.o: backend.c backend.h message.h defs.h

message.o: message.c message.h defs.h

utils.o: utils.c types.h message.h

tree.o: tree.c tree.h types.h defs.h encode.h

encode.o: encode.c encode.h defs.h

gram.o : gram.y $(PPC3H)
	$(YACC) $(YFLAGS) gram.y
	$(CC) $(CFLAGS) -c y.tab.c
	mv y.tab.o gram.o

scan.o : scan.l gram.o $(PPC3H)
	$(LEX) scan.l
	$(CC) $(CFLAGS) -c lex.yy.c
	rm lex.yy.c
	mv lex.yy.o scan.o

y.output: gram.y
	$(YACC) -v -y gram.y

clean:
	-rm -f ppc3 *.o y.tab.h y.output y.tab.c *~

