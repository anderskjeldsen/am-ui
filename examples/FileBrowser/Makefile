# place amlc.jar in this folder or change value.
AMLC=amlc.jar
CMD=java -jar $(AMLC)

build:
	$(CMD) build . -acmd -bt amigaos_docker

build-force-deps:
	$(CMD) build . -acmd -fld -bt amigaos_docker

test:
	$(CMD) test . -bt amigaos_docker
