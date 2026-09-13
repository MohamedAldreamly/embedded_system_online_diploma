#prepared by mohamed aldreamly

CC=arm-none-eabi-
CFLAGS=-mcpu=cortex-m3 -gdwarf-2 -g
INCS=-I inc
LIBS=
SRC=$(wildcard src/*.c)#find all .c file
EXT_SRC=$(wildcard *.c)
OBJ=$(patsubst src/%.c,Debug/%.o,$(SRC))#replace .c file to .o 
EXT_OBJ=$(patsubst %.c,Debug/%.o,$(EXT_SRC))
As=$(wildcard src/*.s)
AsOBJ=$(patsubst src/%.s,Debug/%.o,$(As))
Project_name=project1
BUILD_DIR=Debug

all: $(BUILD_DIR)/$(Project_name).bin
	@echo "=====Bilud is Done====="

$(BUILD_DIR)/%.o: src/%.s | $(BUILD_DIR)
	$(CC)as.exe $(CFLAGS) $< -o $@ 2> log

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC)gcc.exe $(CFLAGS) $(INCS) -c $< -o $@

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC)gcc.exe $(CFLAGS) $(INCS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)	

$(BUILD_DIR)/$(Project_name).elf: $(OBJ) $(EXT_OBJ) $(AsOBJ)
	$(CC)ld.exe -T linker_script.ld $(LIBS) $(EXT_OBJ) $(OBJ) $(AsOBJ) -o $@ -Map=$(BUILD_DIR)/Map_file.map
	cp $(BUILD_DIR)/$(Project_name).elf $(BUILD_DIR)/$(Project_name).axf
	
$(BUILD_DIR)/$(Project_name).bin: $(BUILD_DIR)/$(Project_name).elf
	$(CC)objcopy.exe -O binary $< $@

clean_All:
	rm -rf $(BUILD_DIR)
	@echo "Everything clean"

clean:
	rm -f $(BUILD_DIR)/*.elf $(BUILD_DIR)/*.bin
	@echo ".elf and .bin files clean"
	