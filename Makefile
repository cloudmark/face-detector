CC=gcc
CFLAGS=-arch x86_64 

all: main

main: face_detect.o
	${CC} ${CFLAGS} -L/opt/local/lib face_detect.o -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -o main 
	
face_detect.o: face_detect.c
	${CC} ${CFLAGS} -I/opt/local/include -I/opt/local/include/opencv -c face_detect.c -o face_detect.o
	
clean:
	-rm -rf *o main

rebuild: clean main
