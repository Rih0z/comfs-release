all:
	gcc -Wall server.c  `pkg-config fuse --cflags --libs` -o server -lm  
	gcc -Wall client.c `pkg-config fuse --cflags --libs` -o client -lm  
	gcc -Wall cgoogle.c `pkg-config fuse --cflags --libs` -o cgoogle -lm  
clean:
	rm *.o $(TARGET1) 
hello.o: hello.c
