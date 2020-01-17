CC = gcc
CFLAGS = -std=gnu11

assn1q3: assn1q3.o parse.o prover.o rules.o sequent.o
	$(CC) -o $@ $^ $(CFLAGS)

debug: assn1q3.o parser.o prover.o rules.o sequent.o
	$(CC) -o assn1q3 $^ $(CFLAGS) -g -DDBUG

clean:
	rm -f *.o assn1q3