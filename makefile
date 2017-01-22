PREPROCESSOR_OPTIONS = -D UARM_MACHINE_COMPILING=1 -I/usr/include/uarm -I$(CURDIR) -include stdint.h
 
all: test1

test1: tcbImpl.o pcbImpl.o msgqImpl.o p1test.o 
	arm-none-eabi-ld -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o test1 /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o tcbImpl.o pcbImpl.o msgqImpl.o p1test.o 
	
tcbImpl.o: 
	arm-none-eabi-gcc -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o tcbImpl.o tcbImpl.c
	
pcbImpl.o: 
	arm-none-eabi-gcc -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o pcbImpl.o pcbImpl.c

msgqImpl.o: 
	arm-none-eabi-gcc -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o msgqImpl.o msgqImpl.c
	
p1test.o: 
	arm-none-eabi-gcc -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o p1test.o p1test.c
