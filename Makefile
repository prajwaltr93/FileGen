all: Makefile filegen.c
	@echo "creating filegen executable"
	$(CC) -o filegen filegen.c

clean:
	rm -rf filegen
