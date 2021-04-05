parser: main.o util.o scan.o parse.o
	gcc main.o util.o scan.o parse.o -o parser -lfl

main.o: main.c globals.h util.h scan.h
	gcc -c main.c

util.o: util.c util.h globals.h
	gcc -c util.c

scan.o: lex.yy.c scan.h util.h globals.h
	gcc -c lex.yy.c -o scan.o

lex.yy.c: tiny.l
	flex tiny.l

tiny.tab.c: tiny.y
	bison -d tiny.y

parse.o: tiny.tab.c parse.h scan.h globals.h util.h
	gcc -c tiny.tab.c -o parse.o
clean: 
	-rm lex.yy.c
	-rm tiny.tab.c
	-rm parse.o
	-rm scan.o
	-rm util.o
	-rm main.o
	-rm parser

