#
# paths
#
JAVA_INCLUDE = /usr/lib/jvm/java-6-sun/include
LIBIPFIX_PATH = ../libipfix/lib
JIPFIX_JAVA_SOURCE_PATH = ./src/java/de/fraunhofer/fokus/ants/jipfix

# 
# compiler / linker flags
#
CC = gcc
CFLAGS = -g -Wall -I. -I./src/c -I$(LIBIPFIX_PATH) -I$(JAVA_INCLUDE) -I$(JAVA_INCLUDE)/linux
LDFLAGS = -L$(LIBIPFIX_PATH) -lipfix -lmisc
JAVAC = javac
CLASSPATH = src/java/:./lib/xstream-1.2.jar

JAVA_FILES = $(wildcard $(JIPFIX_JAVA_SOURCE_PATH)/*.java $(JIPFIX_JAVA_SOURCE_PATH)/data/*.java) 
CLASS_FILES = $(patsubst %.java, %.class, $(JAVA_FILES))

.PHONY: all clean

all: lib/libjipfix.so lib/jipfix-0.1.jar

%.class: %.java
	$(JAVAC) -classpath $(CLASSPATH) $?

lib/jipfix-0.1.jar: $(CLASS_FILES)
	cd src/java/ && jar cf ../../$@ $(subst src/java/,,$(CLASS_FILES))

lib/libjipfix.so: src/c/libjipfix.c src/c/de_fraunhofer_fokus_ants_jipfix_jIPFIX.h
	$(CC) $(CFLAGS) $< --shared -o $@ $(LDFLAGS)

ipfix_fields_fokus.xml: make-IPFIX_FIELDS_FOKUS_XML.awk ../libipfix/lib/ipfix_FOKUS_IEs.txt
	awk -f $^ > $@
	

clean:
	rm -f lib/libjipfix.so
	rm -f $(CLASS_FILES) 
	rm -f lib/jipfix-0.1.jar
	