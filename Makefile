# place amlc.jar in this folder or change value.
AMLC:=amlc.jar
CMD=java -jar $(AMLC)
LOGLEVEL:=1
MAXONEERROR:=false
RUNTIMELOGGING:=false
# Local-package-path root: when set, transitive github-realm deps
# (am-lang-core, am-imaging, …) resolve from <LPP>/<id>/ instead of
# the cached dependencies/<id>/latest/ snapshot. Lets you build against
# in-tree changes without manually rsyncing into dependencies/.
LPP:=/Users/anderskjeldsen/Projects/AmLang

build:
	$(CMD) build . -bt linux-x64 -ll 5 -maxOneError -fld

# Same as `build` but with -lpp pointing at $(LPP) so unpushed local
# changes to am-lang-core / am-imaging are picked up directly.
build-lpp:
	$(CMD) build . -bt linux-x64 -ll 5 -maxOneError -fld -lpp $(LPP)

build-amigaos:
	$(CMD) build . -bt amigaos_docker -ll5

build-force-deps:
	$(CMD) build . -fld -bt linux-x64 ll 4

test:
# if MAXONEERROR is true, add -maxOneError flag
ifeq ($(MAXONEERROR),true)
	$(CMD) test . -bt linux-x64 -ll $(LOGLEVEL) -maxOneError
else
	$(CMD) test . -bt linux-x64 -ll $(LOGLEVEL)
endif

# Headless graphics tests — runs locally with gcc, paints into a 150x100
# byte buffer, asserts pixels via the test-only HeadlessPainter class.
test-sim:
	$(CMD) test . -bt amigaos-sim -ll $(LOGLEVEL)

test-rl:
# if MAXONEERROR is true, add -maxOneError flag
ifeq ($(MAXONEERROR),true)
	$(CMD) test . -bt linux-x64 -ll $(LOGLEVEL) -maxOneError -rl -rlarc
else
	$(CMD) test . -bt linux-x64 -ll $(LOGLEVEL) -rl -rlarc
endif

lint:
	$(CMD) lint .


# Debug targets
gdb-test:
	gdb -batch -ex "set environment MALLOC_CHECK_=2" -ex "run" -ex "bt" -ex "quit" builds/test-bin/linux-x64/test_app

gdb-test-interactive:
	gdb builds/test-bin/linux-x64/test_app

gdb-app:
	gdb -batch -ex "run" -ex "bt" -ex "quit" builds/bin/linux-x64/app

gdb-app-interactive:
	gdb builds/bin/linux-x64/app

# Run test with high verbosity for debugging
test-verbose:
	$(CMD) test . -bt linux-x64 -ll 5

# Run test executable directly (bypassing compiler wrapper)
test-direct:
	./builds/test-bin/linux-x64/test_app
