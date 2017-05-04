TST=TestConfigParser\


all: clean
	javac -d build src/*.java tst/*.java
	java -ea -cp build LancerTests $(TST)

client: clean
	javac -d build src/*.java tst/*.java
	java -cp build Client

clean:
	rm -rf build*/*
	rm -rf src/#* src/*~
	rm -rf tst/#* tst/*~

