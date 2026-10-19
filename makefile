AS = nasm
LD = ld
CC = gcc

CFLAGS  = -m32 -ffreestanding -fno-pic -Wall -Wextra -O0 -Isrc/includes -std=gnu99
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
       $(BUILD)/games.o \
       $(BUILD)/ata.o \
       $(BUILD)/fat12.o \
       $(BUILD)/fs.o 

all: $(BUILD) $(BUILD)/kernel.bin

$(BUILD):
	mkdir -p $(BUILD)

# Bootloader da boot/
$(BUILD)/boot.o: boot/boot.s
	$(AS) $(ASFLAGS) boot/boot.s -o $@
	@echo "> boot.s assemblato"

# Kernel da src/kernel/fs/
$(BUILD)/kernel.o: src/kernel/fs/kernel.c
	$(CC) $(CFLAGS) -c src/kernel/fs/kernel.c -o $@
	@echo "> kernel.c compilato"

# Librerie da src/lib/
$(BUILD)/stout.o: src/lib/stout.c src/includes/stout.h
	$(CC) $(CFLAGS) -c src/lib/stout.c -o $@
	@echo "> stout.c compilato"

$(BUILD)/stdfuncs.o: src/lib/stdfuncs.c src/includes/stdfuncs.h
	$(CC) $(CFLAGS) -c src/lib/stdfuncs.c -o $@
	@echo "> stdfuncs.c compilato"

# Driver da src/kernel/drivers/
$(BUILD)/keyboard.o: src/kernel/drivers/keyboard.c src/includes/keyboard.h
	$(CC) $(CFLAGS) -c src/kernel/drivers/keyboard.c -o $@
	@echo "> keyboard.c compilato"

$(BUILD)/time.o: src/kernel/drivers/time.c src/includes/time.h
	$(CC) $(CFLAGS) -c src/kernel/drivers/time.c -o $@
	@echo "> time.c compilato"

$(BUILD)/ata.o: src/kernel/drivers/ata.c src/includes/ata.h
	$(CC) $(CFLAGS) -c src/kernel/drivers/ata.c -o $@
	@echo "> ata.c compilato"

# Filesystem da src/kernel/fs/
$(BUILD)/fat12.o: src/kernel/fs/fat12.c src/includes/fat12.h
	$(CC) $(CFLAGS) -c src/kernel/fs/fat12.c -o $@
	@echo "> fat12.c compilato"

# Programmi utente da src/user/
$(BUILD)/shell.o: src/user/shell.c src/includes/shell.h
	$(CC) $(CFLAGS) -c src/user/shell.c -o $@
	@echo "> shell.c compilato"

$(BUILD)/games.o: src/user/games.c src/includes/games.h
	$(CC) $(CFLAGS) -c src/user/games.c -o $@
	@echo "> games.c compilato"

# Filesystem image incorporata
$(BUILD)/fs.o: fs.img
	$(LD) -m elf_i386 -r -b binary fs.img -o $(BUILD)/fs.o

$(BUILD)/kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $(BUILD)/kernel.bin $(OBJS)
	@echo "> kernel.bin linkato in $(BUILD)/"
	@echo ">>> COMPILAZIONE AVVENUTA <<<"

fs.img:
	dd if=/dev/zero of=fs.img bs=512 count=2880
	mkfs.fat -F 12 fs.img

run: $(BUILD)/kernel.bin fs.img
	qemu-system-i386 -kernel build/kernel.bin -vga std \
			 -drive file=fs.img,format=raw,if=ide \
			 -audiodev sdl,id=audio0 -machine pcspk-audiodev=audio0

clean:
	rm -rf $(BUILD) fs.img
	@echo "> pulito"

.PHONY: all run clean