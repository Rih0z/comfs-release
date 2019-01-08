# comfs-release

this filesystem make you easy to make network communication program

fopen fclose fread fwriteなどの基本的なファイル操作の関数によって通信を確立することができるファイルシステム

please see  
fclient.c 
this is example of client program.


動かし方
how to use 

cd server
make
./server
cd ../
cd mac //or ubuntu
mkdir m// m is (mount point)
./comfs m
cd client
./client

