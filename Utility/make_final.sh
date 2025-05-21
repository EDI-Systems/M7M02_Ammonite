
rm -rf ./M7M02_Final/rvgen.exe

# run in parallel

# build bincopy - unnecessary to do that every time
#(
#( cd ./M7M02_Bincopy && make all ) &&
#./M7M02_Bincopy/upx.exe -9 ./M7M02_Bincopy/bincopy.exe --overlay=strip --strip-relocs=0 -o ./M7M02_Final/bincopy.exe &&
#( cd ./M7M02_Bincopy && make clean )
#) &

# compress files
(./M7M02_Bincopy/upx.exe -9 ./M7M02_Projgen/Release/rvgen.exe --overlay=strip --strip-relocs=0 -o ./M7M02_Final/rvgen.exe) &

wait
echo "All compression ended, please exit now."