build:
	gcc copy_file_thread.c -o thread -lpthread
	gcc copy_file_process.c -o process
	dd if=/dev/urandom of=input.txt bs=1M count=1000
test:
	time ./thread
	time ./process
	time cp input.txt output.txt
cleantxt:
	rm -rf *.txt
clean:
	rm -rf thread process *.txt
check:
	md5sum output* input.txt
