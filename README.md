# xv6-loongarch

cross-tools can be found in : 
https://github.com/loongson/build-tools/

qemu-system-loongarch64 and efi-virtio.rom can be found in :
https://github.com/foxsen/qemu-loongarch-runenv

loongarch_bios.bin compiled from :
https://github.com/yangxiaojuan-loongson/qemu/tree/tcg-dev


input follow command to run xv6 on loongarch:

$ . setenv.sh

$ make qemu

input this command to test Lab syscall in  :
https://pdos.csail.mit.edu/6.828/2021/labs/syscall.html

$ ./grade-lab-syscall

