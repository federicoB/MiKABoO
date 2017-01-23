PREPROCESSOR_OPTIONS = -D UARM_MACHINE_COMPILING=1 -I/usr/include/uarm -I$(CURDIR) -include stdint.h
BUILD_DIR = build
 
all: test1

test1: $(BUILD_DIR)/tcbImpl.o $(BUILD_DIR)/pcbImpl.o $(BUILD_DIR)/msgqImpl.o $(BUILD_DIR)/p1test.o
	arm-none-eabi-ld -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o test1 /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o $(BUILD_DIR)/tcbImpl.o $(BUILD_DIR)/pcbImpl.o $(BUILD_DIR)/msgqImpl.o $(BUILD_DIR)/p1test.o
	
$(BUILD_DIR)/tcbImpl.o: tcbImpl.c mikabooq.h const.h listx.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/tcbImpl.o tcbImpl.c
	
$(BUILD_DIR)/pcbImpl.o: pcbImpl.c mikabooq.h const.h listx.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/pcbImpl.o pcbImpl.c

$(BUILD_DIR)/msgqImpl.o: msgqImpl.c mikabooq.h const.h listx.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/msgqImpl.o msgqImpl.c
	
$(BUILD_DIR)/p1test.o: p1test.c mikabooq.h const.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/p1test.o p1test.c

clean:
	rm -r build
