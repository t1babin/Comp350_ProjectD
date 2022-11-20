gcc -o loadFile loadFile.c
dd if=/dev/zero of=diskc.img bs=512 count=256
nasm bootload.asm
dd if=bootload of=diskc.img conv=notrunc

bcc -ansi -c -o kernel_c.o kernel.c
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel_c.o kernel_asm.o
./loadFile kernel
./loadFile message.txt

bcc -ansi -c -o tstpr1.o tstpr1.c
as86 -o userlib.o userlib.asm
ld86 -d -o tstpr1 tstpr1.o userlib.o
./loadFile tstpr1

bcc -ansi -c -o tstpr2.o tstpr2.c
as86 -o userlib.o userlib.asm
ld86 -d -o tstpr2 tstpr2.o userlib.o
./loadFile tstpr2


bcc -ansi -c -o shell.o shell.c
as86 -o userlib.o userlib.asm
ld86 -d -o shell shell.o userlib.o
./loadFile shell
