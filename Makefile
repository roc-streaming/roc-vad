NUM_CPU ?= $(shell sysctl -n hw.logicalcpu 2>/dev/null || echo 1)
DESTDIR ?=

all: build

.PHONY: build
build:
	mkdir -p build
	if [ ! -e build/3rdparty/bootstrap.commit ]; then \
		cd build ;\
		cmake -DBOOTSTRAP=ON .. ;\
		make --no-print-directory -j$(NUM_CPU) ;\
	fi
	cd build && cmake -DBOOTSTRAP=OFF ..
	cd build && make --no-print-directory -j$(NUM_CPU)

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
	mkdir -p $(DESTDIR)/usr/local/bin/
	cp bin/roc-vad $(DESTDIR)/usr/local/bin/
	mkdir -p $(DESTDIR)/Library/Audio/Plug-Ins/HAL/
	cp -a bin/roc_vad.driver $(DESTDIR)/Library/Audio/Plug-Ins/HAL/

uninstall:
	rm -f $(DESTDIR)/usr/local/bin/roc-vad
	rm -rf $(DESTDIR)/Library/Audio/Plug-Ins/HAL/roc_vad.driver

dist:
	rm -rf build/dist
	mkdir build/dist
	$(MAKE) install DESTDIR=build/dist
	tar --xattrs --uname=root --uid=0 --gname=wheel --gid=0 -s /.// \
		-C build/dist -caPvf roc-vad.tar.bz2 .
	ls -lh roc-vad.tar.bz2

fmt:
	find -type f -name '*.[ch]pp' -not -name '*.pb.*' \
		-not -path './3rdparty/*' -not -path './build/*' \
		| xargs clang-format --verbose -i

docs:
	build/3rdparty/grpc/bin/protoc \
		--plugin=protoc-gen-doc="$$(go env GOPATH)"/bin/protoc-gen-doc \
		--doc_out=. \
		--doc_opt=markdown,RPC.md \
		rpc/*.proto

kick: kickstart
kickstart:
	launchctl kickstart -k system/com.apple.audio.coreaudiod

syslog:
	log stream --predicate 'sender == "roc_vad"'
