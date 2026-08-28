AS = nasm
LD = ld
CC = gcc

CFLAGS  = -m32 -ffreestanding -fno-pic -Wall -Wextra -O0 -Ikernel/includes -std=gnu99
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

BUILD = build

OBJS = $(BUILD)/boot.o \
       $(BUILD)/kernel.o \
       $(BUILD)/stout.o \
       $(BUILD)/keyboard.o \
       $(BUILD)/time.o \
       $(BUILD)/shell.o \
	   $(BUILD)/stdfuncs.o \
       $(BUILD)/fs.o 

all: $(BUILD) $(BUILD)/kernel.bin

$(BUILD):
	mkdir -p $(BUILD)

# Bootloader da boot/
$(BUILD)/boot.o: boot/boot.s
	$(AS) $(ASFLAGS) boot/boot.s -o $@
	@echo "> boot.s assemblato"

# Kernel da kernel/
$(BUILD)/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c kernel/kernel.c -o $@
	@echo "> kernel.c compilato"

# Driver da kernel/drivers/ e include da kernel/includes/
$(BUILD)/stout.o: kernel/drivers/stout.c kernel/includes/stout.h
	$(CC) $(CFLAGS) -c kernel/drivers/stout.c -o $@
	@echo "> stout.c compilato"

$(BUILD)/keyboard.o: kernel/drivers/keyboard.c kernel/includes/keyboard.h
	$(CC) $(CFLAGS) -c kernel/drivers/keyboard.c -o $@
	@echo "> keyboard.c compilato"

$(BUILD)/time.o: kernel/drivers/time.c kernel/includes/time.h
	$(CC) $(CFLAGS) -c kernel/drivers/time.c -o $@
	@echo "> time.c compilato"

$(BUILD)/shell.o: kernel/drivers/shell.c kernel/includes/shell.h
	$(CC) $(CFLAGS) -c kernel/drivers/shell.c -o $@
	@echo "> shell.c compilato"

$(BUILD)/fs.o: fs.img
	$(LD) -m elf_i386 -r -b binary fs.img -o $(BUILD)/fs.o

$(BUILD)/stdfuncs.o: kernel/drivers/stdfuncs.c kernel/includes/stdfuncs.h
	$(CC) $(CFLAGS) -c kernel/drivers/stdfuncs.c -o $@
	@echo "> stdfuncs.c compilato"

$(BUILD)/kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $(BUILD)/kernel.bin $(OBJS)
	@echo "> kernel.bin linkato in $(BUILD)/"
	@echo ">>> COMPILAZIONE AVVENUTA <<<"

fs.img:
	dd if=/dev/zero of=fs.img bs=512 count=2880
	mkfs.fat -F 12 fs.img

run: $(BUILD)/kernel.bin fs.img
	qemu-system-i386 -kernel $(BUILD)/kernel.bin -vga std \
	                 -drive file=fs.img,format=raw,if=ide

clean:
	rm -rf $(BUILD) fs.img
	@echo "> pulito"

.PHONY: all run clean