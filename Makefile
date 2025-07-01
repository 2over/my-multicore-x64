BUILD:=./build

CFLAGS:= -m32 # 32 位的程序
CFLAGS+= -masm=intel
CFLAGS+= -fno-builtin	# 不需要 gcc 内置函数
CFLAGS+= -nostdinc		# 不需要标准头文件
CFLAGS+= -fno-pic		# 不需要位置无关的代码  position independent code
CFLAGS+= -fno-pie		# 不需要位置无关的可执行程序 position independent executable
CFLAGS+= -nostdlib		# 不需要标准库
CFLAGS+= -fno-stack-protector	# 不需要栈保护
CFLAGS:=$(strip ${CFLAGS})

DEBUG:= -g

HD_IMG_NAME:= "hd.img"

all: ${BUILD}/boot/boot.o ${BUILD}/boot/setup.o ${BUILD}/system.bin ${BUILD}/x64/x64.o ${BUILD}/kernel64/system.bin
	$(shell rm -rf $(BUILD)/$(HD_IMG_NAME))
	bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $(BUILD)/$(HD_IMG_NAME)
	dd if=${BUILD}/boot/boot.o of=$(BUILD)/$(HD_IMG_NAME) bs=512 seek=0 count=1 conv=notrunc
	dd if=${BUILD}/boot/setup.o of=$(BUILD)/$(HD_IMG_NAME) bs=512 seek=1 count=2 conv=notrunc
	dd if=${BUILD}/system.bin of=$(BUILD)/$(HD_IMG_NAME) bs=512 seek=3 count=30 conv=notrunc
	dd if=${BUILD}/kernel64/system.bin of=$(BUILD)/$(HD_IMG_NAME) bs=512 seek=41 count=5000 conv=notrunc

# 下面是64位内核用的
${BUILD}/kernel64/system.bin: ${BUILD}/kernel64/kernel.bin
	objcopy -O binary ${BUILD}/kernel64/kernel.bin ${BUILD}/kernel64/system.bin
	nm ${BUILD}/kernel64/kernel.bin | sort > ${BUILD}/kernel64/system.map

${BUILD}/kernel64/kernel.bin: ${BUILD}/kernel64/boot/head.o ${BUILD}/kernel64/init/main64.o
	ld -b elf64-x86-64 -o $@ $^ -Ttext 0x100000

${BUILD}/kernel64/boot/%.o: x64kernel/boot/%.asm
	$(shell mkdir -p ${BUILD}/kernel64/boot)
	nasm -f elf64 ${DEBUG} $< -o $@

${BUILD}/kernel64/init/%.o: x64kernel/init/%.c
	$(shell mkdir -p ${BUILD}/kernel64/init)
	gcc ${DEBUG} ${CFLAGS64} -c $< -o $@

# 下面是32位内核用的
${BUILD}/system.bin: ${BUILD}/kernel.bin
	objcopy -O binary ${BUILD}/kernel.bin ${BUILD}/system.bin
	nm ${BUILD}/kernel.bin | sort > ${BUILD}/system.map

${BUILD}/kernel.bin: ${BUILD}/boot/head.o ${BUILD}/init/main.o ${BUILD}/kernel/asm/io.o ${BUILD}/kernel/chr_drv/console.o \
    ${BUILD}/lib/string.o ${BUILD}/kernel/vsprintf.o ${BUILD}/kernel/printk.o ${BUILD}/init/enter_x64.o ${BUILD}/kernel/kernel.o \
    ${BUILD}/kernel/gdt.o
	ld -m elf_i386 $^ -o $@ -Ttext 0x1200

${BUILD}/init/enter_x64.o: oskernel/init/enter_x64.asm
	$(shell mkdir -p ${BUILD}/init)
	nasm -f elf32 -g $< -o $@

${BUILD}/kernel/%.o: oskernel/kernel/%.c
	$(shell mkdir -p ${BUILD}/kernel)
	gcc ${CFLAGS} ${DEBUG} -c $< -o $@

${BUILD}/lib/%.o: oskernel/lib/%.c
	$(shell mkdir -p ${BUILD}/lib)
	gcc ${CFLAGS} ${DEBUG} -c $< -o $@

${BUILD}/kernel/chr_drv/%.o: oskernel/kernel/chr_drv/%.c
	$(shell mkdir -p ${BUILD}/kernel/chr_drv)
	gcc ${CFLAGS} ${DEBUG} -c $< -o $@

${BUILD}/kernel/asm/%.o: oskernel/kernel/asm/%.asm
	$(shell mkdir -p ${BUILD}/kernel/asm)
	nasm -f elf32 -g $< -o $@

${BUILD}/init/main.o: oskernel/init/main.c
	$(shell mkdir -p ${BUILD}/init)
	gcc ${CFLAGS} ${DEBUG} -c $< -o $@

${BUILD}/boot/head.o: oskernel/boot/head.asm
	nasm -f elf32 -g $< -o $@

${BUILD}/boot/%.o: oskernel/boot/%.asm
	$(shell mkdir -p ${BUILD}/boot)
	nasm $< -o $@

clean:
	$(shell rm -rf ${BUILD})

bochs: all
	bochs -q -f bochsrc

qemug: all
	qemu-system-x86_64 \
	-m 32M \
	-boot c \
	-cpu Haswell -smp cores=1,threads=2 \
	-hda ./build/hd.img \
	-s -S

qemu: all
	qemu-system-x86_64 \
	-m 32M \
	-boot c \
	-cpu Haswell -smp cores=1,threads=2 \
	-hda ./build/hd.img

# 生成的内核镜像给VBox、VMware用
vmdk: $(BUILD)/master.vmdk

$(BUILD)/master.vmdk: ./build/hd.img
	qemu-img convert -O vmdk $< $@