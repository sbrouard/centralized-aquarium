TST=TestConfigParser.java\


all: clean
	javac -d build src/*.java tst/*.java
	java -ea -cp build LancerTests $(TST)

clean:
	rm -rf build*/*
	rm -rf src/#* src/*~
	rm -rf tst/#* tst/*~

