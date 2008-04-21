#!/bin/sh

java -Xcheck:jni -cp lib/jipfix-0.1.jar:lib/xstream-1.2.jar:lib/xpp3-1.1.3.4.O.jar -Djava.library.path=./lib/ de.fraunhofer.fokus.ants.jipfix.jIPFIX
