PREPROCESSOR_OPTIONS = -D UARM_MACHINE_COMPILING=1 -I/usr/include/uarm -I$(CURDIR)
BUILD_DIR = build
DIST_DIR = dist
UARM_OBJECT_FILES = /usr/include/uarm/crtso.o /usr/include/uarm/libuarm.o
KERNEL_STRUCT_OBJ = $(UARM_OBJECT_FILES) $(BUILD_DIR)/tcbImpl.o $(BUILD_DIR)/pcbImpl.o $(BUILD_DIR)/msgqImpl.o
PHASE_2_OBJ = $(KERNEL_STRUCT_OBJ) $(BUILD_DIR)/init.o $(BUILD_DIR)/scheduler.o $(BUILD_DIR)/interrupt.o $(BUILD_DIR)/exception.o $(BUILD_DIR)/ssi.o $(BUILD_DIR)/p2test.o $(BUILD_DIR)/utils.o /usr/include/uarm/libdiv.o
DEBUG_FLAGS = -g
 
all: $(DIST_DIR)/test1 $(DIST_DIR)/test2

$(DIST_DIR)/test1: $(KERNEL_STRUCT_OBJ) $(BUILD_DIR)/p1test.o
	mkdir -p $(DIST_DIR)
	arm-none-eabi-ld -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o $(DIST_DIR)/test1 $(KERNEL_STRUCT_OBJ) $(BUILD_DIR)/p1test.o

$(DIST_DIR)/test2: $(PHASE_2_OBJ)
	mkdir -p $(DIST_DIR)
	arm-none-eabi-ld -T /usr/include/uarm/ldscripts/elf32ltsarm.h.uarmcore.x -o $(DIST_DIR)/test2 $(PHASE_2_OBJ)
	
$(BUILD_DIR)/tcbImpl.o: tcbImpl.c mikabooq.h const.h listx.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc $(DEBUG_FLAGS) -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/tcbImpl.o tcbImpl.c
	
$(BUILD_DIR)/pcbImpl.o: pcbImpl.c mikabooq.h const.h listx.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc $(DEBUG_FLAGS) -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/pcbImpl.o pcbImpl.c

$(BUILD_DIR)/msgqImpl.o: msgqImpl.c mikabooq.h const.h listx.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc $(DEBUG_FLAGS) -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/msgqImpl.o msgqImpl.c

$(BUILD_DIR)/scheduler.o: scheduler.c scheduler.h mikabooq.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc $(DEBUG_FLAGS) -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/scheduler.o scheduler.c

$(BUILD_DIR)/init.o: init.c scheduler.h mikabooq.h utils.h exception.h interrupt.h ssi.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc $(DEBUG_FLAGS) -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/init.o init.c
	
$(BUILD_DIR)/p1test.o: p1test.c mikabooq.h const.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc $(DEBUG_FLAGS) -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/p1test.o p1test.c

$(BUILD_DIR)/p2test.o: p2test.c
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc $(DEBUG_FLAGS) -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/p2test.o p2test.c

$(BUILD_DIR)/exception.o: exception.c exception.h scheduler.h utils.h ssi.h mikabooq.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc $(DEBUG_FLAGS) -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/exception.o exception.c

$(BUILD_DIR)/interrupt.o: interrupt.c interrupt.h scheduler.h utils.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc $(DEBUG_FLAGS) -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/interrupt.o interrupt.c

$(BUILD_DIR)/ssi.o: ssi.c ssi.h utils.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc $(DEBUG_FLAGS) -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/ssi.o ssi.c

$(BUILD_DIR)/utils.o: utils.c utils.h
	mkdir -p $(BUILD_DIR)
	arm-none-eabi-gcc $(DEBUG_FLAGS) -mcpu=arm7tdmi -c $(PREPROCESSOR_OPTIONS) -o $(BUILD_DIR)/utils.o utils.c

clean:
	rm -r ./$(BUILD_DIR)
