.PHONY: all,clean,load

CC = arm-none-eabi-gcc
OBJC = arm-none-eabi-objcopy
OBJD = arm-none-eabi-objdump
AS = arm-none-eabi-as
LD = arm-none-eabi-ld

TARGET = src/main.cpp
INC = inc/
LIB = lib/
FRS = freeRTOS/src/
FRH = freeRTOS/inc/

CPPFLAGS = -c -g -O0 -Wall -nostartfiles -lstdc++ \
	-mcpu=cortex-m3 -mthumb -march=armv7-m -Wno-pointer-arith -mfloat-abi=soft \
	-ffast-math -fno-math-errno -ffunction-sections -fdata-sections \
	-fno-rtti -fno-exceptions -specs=nosys.specs -specs=nano.specs -fno-common -D"assert_param(x)=" \
	-L/usr/lib/arm-none-eabi/newlib/ #-u_printf_float
CFLAGS = -Wall -g -O0 -specs=nosys.specs -specs=nano.specs \
	 -c -fno-common -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -Wno-pointer-arith 
LCPPFLAGS = -mcpu=cortex-m3 -mthumb -nostartfiles -lm -lc -lgcc \
		 -specs=nano.specs -specs=nosys.specs -fno-exceptions -fno-rtti \
		 -mfloat-abi=soft -fno-use-cxa-atexit \
		 -L/usr/lib/arm-none-eabi/newlib/thumb/v7-m/ -L/usr/lib/arm-none-eabi/newlib/ \
		 	-Xlinker -Map=main.map -z muldefs 
LDFLAGS =  -marmelf --gc-sections -lgcc -lm -lc \
	-L/usr/lib/gcc/arm-none-eabi/7.3.1/ -L/usr/lib/gcc/arm-none-eabi/7.3.1/thumb/v7-m/
	
load: main.bin
	#st-info --probe
	st-flash write main.bin 0x08000000

all: main.bin main.elf main.lst
main.bin: main.elf
	$(OBJC) main.elf main.bin -O binary
main.lst: main.elf
	$(OBJD) -D main.elf > main.lst
main.elf: startup.o usb_device.o normalqueue.o main.o # malloc.o tasks.o port.o queue.o list.o timers.o heap_2.o main.o
	$(CC) -o main.elf -T$(LIB)stm32f107.ld startup.o usb_device.o normalqueue.o main.o \
	-I$(LIB) -I$(FRH) $(LCPPFLAGS)
	arm-none-eabi-size main.elf
startup.o: $(LIB)startup.cpp
	$(CC) $(LIB)startup.cpp -o startup.o $(CPPFLAGS)
usb_device.o: src/usb_device.cpp
	$(CC) src/usb_device.cpp -o usb_device.o -I$(INC) -I$(LIB) $(CPPFLAGS)
normalqueue.o: src/normalqueue.cpp
	$(CC) src/normalqueue.cpp -o normalqueue.o -I$(INC) $(CPPFLAGS)
#malloc.o: src/malloc.cpp
#	$(CC) src/malloc.cpp -o malloc.o -I$(INC) -I$(FRH) $(CPPFLAGS)	
#port.o: freeRTOS/src/port.c 
#	$(CC) freeRTOS/src/port.c -o port.o -I$(FRH) -I$(INC) $(CFLAGS)
#tasks.o: freeRTOS/src/tasks.c
#	$(CC) freeRTOS/src/tasks.c -o tasks.o -I$(FRH) -I$(INC) $(CFLAGS)
#queue.o: freeRTOS/src/queue.c
#	$(CC) freeRTOS/src/queue.c -o queue.o -I$(FRH) -I$(INC) $(CFLAGS)	
#list.o: freeRTOS/src/list.c
#	$(CC) freeRTOS/src/list.c -o list.o -I$(FRH) -I$(INC) $(CFLAGS)	
#timers.o: freeRTOS/src/timers.c
#	$(CC) freeRTOS/src/timers.c -o timers.o -I$(FRH) -I$(INC) $(CFLAGS)	
#heap_2.o: freeRTOS/src/heap_2.c $(INC)
#	$(CC) freeRTOS/src/heap_2.c -o heap_2.o -I$(FRH) -I$(INC) $(CFLAGS)		
main.o: $(TARGET) #$(INC) $(FRH)
	$(CC) $(TARGET) -o main.o -I$(INC) -I$(LIB) -I$(FRH) $(CPPFLAGS)
	
clean:
	rm -rf *.o *.elf *.lst *.bin *.map 


	
