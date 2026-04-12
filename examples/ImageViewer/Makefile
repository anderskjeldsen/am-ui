# place amlc.jar in this folder or change value.
AMLC=amlc.jar
CMD=java -jar $(AMLC)

clean:
	$(CMD) clean . -bt amigaos_docker

builds/bin/amigaos/image.png: assets/image.png
	mkdir -p builds/bin/amigaos
	cp assets/image.png builds/bin/amigaos/image.png

build: builds/bin/amigaos/image.png
	$(CMD) build . -bt amigaos_docker

build-rl: builds/bin/amigaos/image.png
	$(CMD) build . -bt amigaos_docker -rl

build-force-deps: builds/bin/amigaos/image.png
	$(CMD) build . -fld -bt amigaos_docker

test:
	$(CMD) test . -bt amigaos_docker
