NUM_CPU ?= $(shell sysctl -n hw.logicalcpu 2>/dev/null || echo 1)

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

fmt:
	find -type f -name '*.[ch]pp' -not -path './3rdparty/*' -not -name '*.pb.*' \
		| xargs clang-format --verbose -i

install:
	cp bin/roc-vad /usr/local/bin/
	mkdir -p /Library/Audio/Plug-Ins/HAL/
	cp -a bin/roc_vad.driver /Library/Audio/Plug-Ins/HAL/

uninstall:
	rm -f /usr/local/bin/roc-vad
	rm -rf /Library/Audio/Plug-Ins/HAL/roc_vad.driver

kickstart:
	launchctl kickstart -k system/com.apple.audio.coreaudiod
