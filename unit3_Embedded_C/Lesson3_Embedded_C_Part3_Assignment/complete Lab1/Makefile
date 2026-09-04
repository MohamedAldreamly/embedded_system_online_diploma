#prepared by mohamed aldreamly

CC=arm-none-eabi-
CFLAGS=-mcpu=arm926ej-s -g
INCS=-I .
LIBS=
SRC=$(wildcard *.c)#find all .c file
OBJ=$(SRC:.c=.o)#replace .c file to .o 
As=$(wildcard *.s)
AsOBJ=$(As:.s=.o)
Project_name=learn-in-depth

all:$(Project_name).bin
	@echo "=====Bilud is Done====="

%.o: %.s
	$(CC)as.exe $(CFLAGS) $< -o $@ 2> log

%.o: %.c
	$(CC)gcc.exe $(CFLAGS) $(INCS) -c $< -o $@

learn-in-depth.elf: $(OBJ) $(AsOBJ)
	$(CC)ld.exe -T linker_script.ld $(LIBS) $(OBJ) $(AsOBJ) -o $@ -Map=Map_file.map

learn-in-depth.bin: learn-in-depth.elf
	$(CC)objcopy.exe -O binary $< $@

clean_All:
	rm *.o *.elf *.bin
	@echo "Everything clean"

clean:
	rm -rf *.elf *.bin
	@echo ".elf and .bin files clean"