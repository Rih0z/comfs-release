# comfs-release

this filesystem make you easy to make network communication program

fopen fclose fread fwriteなどの基本的なファイル操作の関数によって通信を確立することができるファイルシステム

please see  
fclient.c 
this is example of client program.


動かし方
how to use 

plese install fuse 

mac
brew install osxfuse
https://osxfuse.github.io/

ubuntu
sudo apt-get install libfuse-dev
https://gist.github.com/cstroe/e83681e3510b43e3f618

cd server
make
./server
cd ../
cd mac //or ubuntu
mkdir m// m is (mount point)
./comfs m
./client

