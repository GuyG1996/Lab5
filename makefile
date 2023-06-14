#  Guy Ginat 206922544, Ron Hadad 209260645
all: task2

task2.o: task2.c
	gcc -m32 -c task2.c -o task2.o

start.o : start.s
	nasm -g -f elf32 start.s -o start.o

task2: task2.o startup.o start.o
	ld -o task2 task2.o startup.o start.o -L/usr/lib32 -lc -T linking_script -dynamic-linker /lib32/ld-linux.so.2

clean:
	rm -f task2 task2.o start.o

.PHONY: all clean
