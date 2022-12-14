# Makefile

all :
	rm -f bincopy.exe
	gcc -fsigned-char -O3 -g0 bincopy.c -o bincopy_pre.exe
	./upx -9 ./bincopy_pre.exe --overlay=strip --strip-relocs=0 -o ./bincopy.exe
	rm -f bincopy_pre.exe

clean :
	rm -f bincopy.exe
