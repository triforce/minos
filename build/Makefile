FASM=fasm

all: clean img kernel

img:
	@echo "Building minOS disk image..."
	@$(FASM) ../kernel/kernel.ASM kernel.BIN
	@$(FASM) ../kernel/boot/boot_disk.ASM boot_disk.img
	@mv boot_disk.img ../imgs/minOS.img
	@echo "Built ../imgs/minOS.img"

kernel:
	@echo "Building minOS multiboot kernel..."
	@$(FASM) ../kernel/boot/multiboot_disk.ASM multiboot_disk.BIN
	@$(FASM) ../kernel/kernel.ASM kernel.BIN
	@$(FASM) build_m.ASM build_m.BIN
	@mv build_m.BIN ../grub/minOS
	@echo "Built ../grub/minOS"

clean:
	rm -rf ../imgs/*
	rm -rf ../grub/*
