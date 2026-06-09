AS = nasm
LD = ld
CC = gcc

CFLAGS  = -m32 -ffreestanding -fno-pic -Wall -Wextra -O0
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

BUILD = build

OBJS = $(BUILD)/boot.o \
       $(BUILD)/kernel.o \
       $(BUILD)/stout.o \
       $(BUILD)/keyboard.o \
       $(BUILD)/time.o \
       $(BUILD)/shell.o \
	   $(BUILD)/fs.o 

all: $(BUILD) kernel.bin

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: boot.s
	$(AS) $(ASFLAGS) boot.s -o $@
	@echo "> boot.s assemblato"

$(BUILD)/kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o $@
	@echo "> kernel.c compilato"

$(BUILD)/stout.o: stout.c stout.h
	$(CC) $(CFLAGS) -c stout.c -o $@
	@echo "> stout.c compilato"

$(BUILD)/keyboard.o: keyboard.c keyboard.h
	$(CC) $(CFLAGS) -c keyboard.c -o $@
	@echo "> keyboard.c compilato"

$(BUILD)/time.o: time.c time.h
	$(CC) $(CFLAGS) -c time.c -o $@
	@echo "> time.c compilato"

$(BUILD)/shell.o: shell.c shell.h
	$(CC) $(CFLAGS) -c shell.c -o $@
	@echo "> shell.c compilato"
$(BUILD)/fs.o: fs.img
	$(LD) -m elf_i386 -r -b binary fs.img -o $(BUILD)/fs.o
kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o kernel.bin $(OBJS)
	@echo "> kernel.bin linkato"
	@echo ">>> COMPILAZIONE AVVENUTA <<<"
fs.img:
	dd if=/dev/zero of=fs.img bs=512 count=2880
	mkfs.fat -F 12 fs.img
	# copia i tuoi file dentro:
	# mcopy -i fs.img miofile.txt ::miofile.txt

run: kernel.bin
	fs.img:
	dd if=/dev/zero of=fs.img bs=512 count=2880
	mkfs.fat -F 12 fs.img

run: kernel.bin fs.img
	qemu-system-i386 -kernel kernel.bin -vga std \
	                 -drive file=fs.img,format=raw,if=ide
clean:
	rm -rf $(BUILD) kernel.bin
	@echo "> pulito"

.PHONY: all run clean