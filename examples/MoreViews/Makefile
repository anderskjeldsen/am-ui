# place amlc.jar in this folder or change value.
AMLC=amlc.jar
CMD=java -jar $(AMLC)

build:
	$(CMD) build . -acmd -bt amigaos_docker_unix-x64

build-force-deps:
	$(CMD) build . -acmd -fld -bt amigaos_docker_unix-x64

test:
	$(CMD) test . -bt amigaos_docker_unix-x64