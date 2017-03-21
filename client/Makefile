TST=TestConfigParser\


all: clean
	javac -d build src/*.java tst/*.java
	java -ea -cp build LancerTests $(TST)

client: clean
	javac -d build src/*.java tst/*.java
	java -cp build Client

aquarium: clean
	javac -d build src/*.java tst/*.java
	java -cp build Aquarium

clean:
	rm -rf build*/*
	rm -rf src/#* src/*~
	rm -rf tst/#* tst/*~

