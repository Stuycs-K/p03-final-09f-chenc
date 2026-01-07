.PHONY: run clean compile
run: server
	@./server
compile server: server.o
	@gcc server.o -Wall -o server
server.o: server.c
	@gcc -c server.c
clean:
	rm -f *.o *.gch server
