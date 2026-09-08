#prepared by mohamed aldreamly

CC=arm-none-eabi-
CFLAGS=-mcpu=cortex-m4 -gdwarf-2 -g 
INCS=-I .
LIBS=
SRC=$(wildcard *.c)#find all .c file
OBJ=$(SRC:.c=.o)#replace .c file to .o 
As=$(wildcard *.s)
AsOBJ=$(As:.s=.o)
Project_name=unit3_lab4_cortexM4

all:$(Project_name).bin
	@echo "=====Bilud is Done====="

%.o: %.s
	$(CC)as.exe $(CFLAGS) $< -o $@ 2> log

%.o: %.c
	$(CC)gcc.exe $(CFLAGS) $(INCS) -c $< -o $@

$(Project_name).elf: $(OBJ) $(AsOBJ)
	$(CC)ld.exe -T linker_script.ld $(LIBS) $(OBJ) $(AsOBJ) -o $@ -Map=Map_file.map
	cp $(Project_name).elf $(Project_name).axf
$(Project_name).bin: $(Project_name).elf
	$(CC)objcopy.exe -O binary $< $@

clean_All:
	rm *.o *.elf *.bin
	@echo "Everything clean"

clean:
	rm -rf *.elf *.bin
	@echo ".elf and .bin files clean"