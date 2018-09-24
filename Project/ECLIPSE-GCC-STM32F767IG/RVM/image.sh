arm-none-eabi-objcopy -O binary RVM RVM.bin
arm-none-eabi-objdump -S RVM > RVM.asm
../bincopy -i RVM.bin -o M7M2_Init.c
