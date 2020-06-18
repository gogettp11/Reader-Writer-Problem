problem1: problem1.o problem2.o
	gcc problem1.o -pthread -o problem1
	gcc problem2.o -pthread -o problem2

problem1.o: problem1.c 
	gcc problem1.c -c -pthread

problem2.o: problem2.c 
	gcc problem2.c -c -pthread

clean:
	rm *.o problem1 problem2