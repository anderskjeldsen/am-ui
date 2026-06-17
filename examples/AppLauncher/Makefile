# place amlc.jar in this folder or change value.
AMLC=amlc.jar
CMD=java -jar $(AMLC)

AMIGAOS_BIN=builds/bin/amigaos

clean:
	$(CMD) clean . -bt amigaos_docker

$(AMIGAOS_BIN)/apps.json: assets/apps.json
	mkdir -p $(AMIGAOS_BIN)
	cp assets/apps.json $(AMIGAOS_BIN)/apps.json

copy-icons: $(AMIGAOS_BIN)/apps.json
	@if [ -d assets/icons ]; then \
		cp assets/icons/*.png $(AMIGAOS_BIN)/ 2>/dev/null || true; \
	fi

build: $(AMIGAOS_BIN)/apps.json copy-icons
	$(CMD) build . -bt amigaos_docker

build-rl: $(AMIGAOS_BIN)/apps.json copy-icons
	$(CMD) build . -bt amigaos_docker -rl

build-force-deps: $(AMIGAOS_BIN)/apps.json copy-icons
	$(CMD) build . -fld -bt amigaos_docker

test:
	$(CMD) test . -bt amigaos_docker
