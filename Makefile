inc = $(wildcard *.h)

all: test_clinklist test_fallocator test_timerwheel test_timerservice

test_clinklist: test_clinklist.c clinklist.h
	gcc -o test_clinklist test_clinklist.c

test_fallocator: test_fallocator.c fallocator.c fallocator.h
	gcc -o test_fallocator test_fallocator.c fallocator.c

test_timerwheel: test_timerwheel.c timerwheel.c timerwheel.h clinklist.h
	gcc -o test_timerwheel test_timerwheel.c timerwheel.c

test_timerservice: test_timerservice.c timerservice.c timerwheel.c fallocator.c $(inc)
	gcc -o test_timerservice test_timerservice.c timerservice.c timerwheel.c fallocator.c

.PHONY: clean
clean:
	rm -f test_clinklist test_fallocator test_timerservice test_timerwheel