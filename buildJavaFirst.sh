javac -classpath ./src/java/:./lib/xstream-1.2.jar src/java/de/fraunhofer/fokus/ants/jipfix/*.java src/java/de/fraunhofer/fokus/ants/jipfix/data/*.java || exit 1 
cd src/java/
jar cvf ../../lib/jipfix-0.1.jar de/fraunhofer/fokus/ants/jipfix/*.class de/fraunhofer/fokus/ants/jipfix/data/*.class
cd ../..
