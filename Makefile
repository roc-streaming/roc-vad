NUM_CPU ?= $(shell sysctl -n hw.logicalcpu 2>/dev/null || echo 1)

BUILDDIR ?= build/darwin-$(shell uname -r).noindex
DESTDIR ?=

GTEST_COLOR ?= yes
export GTEST_COLOR

GTEST_BREAK_ON_FAILURE ?= yes
export GTEST_BREAK_ON_FAILURE

all: build

.PHONY: build
build:
	mkdir -p $(BUILDDIR)
	if [ ! -e $(BUILDDIR)/3rdparty/bootstrap.commit ]; then \
		cd $(BUILDDIR) &&\
		cmake -DBOOTSTRAP=ON ../.. &&\
		make --no-print-directory -j$(NUM_CPU) || exit 1;\
	fi
	cd $(BUILDDIR) && cmake -DBOOTSTRAP=OFF ../..
	cd $(BUILDDIR) && make --no-print-directory -j$(NUM_CPU)

.PHONY: test
test: build
	$(BUILDDIR)/test/rocvad_test

clean:
	rm -rf bin build
	rm -f compile_commands.json

info:
	ls -lh bin/roc-vad bin/roc_vad.driver/Contents/MacOS/roc_vad
	@echo
	file bin/roc-vad bin/roc_vad.driver/Contents/MacOS/roc_vad
	@echo
	otool -L bin/roc-vad
	@echo
	otool -L bin/roc_vad.driver/Contents/MacOS/roc_vad
	@echo
	nm -gU bin/roc-vad
	nm -gU bin/roc_vad.driver/Contents/MacOS/roc_vad

install:
	killall roc-vad 2>/dev/null || true
	mkdir -p $(DESTDIR)/usr/local/bin/
	cp bin/roc-vad $(DESTDIR)/usr/local/bin/
	mkdir -p $(DESTDIR)/Library/Audio/Plug-Ins/HAL/
	cp -a bin/roc_vad.driver $(DESTDIR)/Library/Audio/Plug-Ins/HAL/

uninstall:
	rm -f $(DESTDIR)/usr/local/bin/roc-vad
	rm -rf $(DESTDIR)/Library/Audio/Plug-Ins/HAL/roc_vad.driver

dist:
	rm -rf $(BUILDDIR)/dist
	mkdir $(BUILDDIR)/dist
	$(MAKE) install DESTDIR=$(BUILDDIR)/dist
	tar --xattrs --uname=root --uid=0 --gname=wheel --gid=0 -s /.// \
		-C $(BUILDDIR)/dist -caPvf roc-vad.tar.bz2 .
	ls -lh roc-vad.tar.bz2

fmt:
	find -type f -name '*.[ch]pp' -not -name '*.pb.*' -not -path './build/*' \
		| xargs clang-format --verbose -i

rpcmd:
	$(BUILDDIR)/3rdparty/grpc/bin/protoc \
		--plugin=protoc-gen-doc="$$(go env GOPATH)"/bin/protoc-gen-doc \
		--doc_out=. \
		--doc_opt=markdown,RPC.md \
		rpc/*.proto

tocmd:
	markdown-toc --maxdepth 3 -i README.md
	markdown-toc --maxdepth 3 -i HACKING.md

md: tocmd
	md-authors --format modern --append AUTHORS.md

d2:
	d2 docs/classes.d2 docs/classes.svg

kick: kickstart
kickstart:
	launchctl kickstart -k system/com.apple.audio.coreaudiod 2>/dev/null \
		|| killall -v -9 coreaudiod

syslog:
	log stream --predicate 'sender == "roc_vad"'

sysprof:
	system_profiler SPAudioDataType
