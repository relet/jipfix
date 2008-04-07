export JNI_H_PATH=/usr/lib/jvm/java-1.5.0-sun-1.5.0.13/include/
export MY_OS=linux
#export IPFIX_PATH=/usr/local/lib/

gcc -g -Wall -I. -I$JNI_H_PATH -I$JNI_H_PATH/$MY_OS/ -I../lib/ -I./src/c/ ./src/c/libjipfix.c --shared -o ./lib/libjipfix.so -L$IPFIX_PATH -lipfix -lmisc
