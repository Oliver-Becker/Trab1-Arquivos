progname=programaTrab1

all:
	gcc main.c src/*.c -o $(progname) -Iinc/.

run:
	./$(progname)

zip: 
	zip -r $(progname).zip Makefile main.c 

