# place amlc.jar in this folder or change value.
AMLC=../../../amlc.jar
LPP=../../..
CMD=java -jar $(AMLC)

MACOS_ARM_BIN=builds/bin/macos-arm

clean:
	$(CMD) clean . -bt macos-arm

build-macos-arm:
	$(CMD) build . -bt macos-arm -lpp $(LPP)

build-amigaos:
	$(CMD) build . -bt amigaos_docker -lpp $(LPP)
