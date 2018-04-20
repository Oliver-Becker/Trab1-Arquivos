progname=programaTrab1

all:
	gcc main.c -o $(progname) -g

run:
	./$(progname)

zip: 
	zip -r $(progname).zip Makefile main.c 

