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
	ls -lh bin/rcpadm bin/rcp_plugin.driver/Contents/MacOS/rcp_plugin
	@echo
	file bin/rcpadm bin/rcp_plugin.driver/Contents/MacOS/rcp_plugin
	@echo
	otool -L bin/rcpadm
	@echo
	otool -L bin/rcp_plugin.driver/Contents/MacOS/rcp_plugin
	@echo
	nm -gU bin/rcpadm
	nm -gU bin/rcp_plugin.driver/Contents/MacOS/rcp_plugin

fmt:
	find -type f -name '*.[ch]pp' -not -path './3rdparty/*' -not -name '*.pb.*' \
		| xargs clang-format --verbose -i

install:
	cp bin/rcpadm /usr/local/bin/
	mkdir -p /Library/Audio/Plug-Ins/HAL/
	cp -a bin/rcp_plugin.driver /Library/Audio/Plug-Ins/HAL/

uninstall:
	rm -f /usr/local/bin/rcpadm
	rm -rf /Library/Audio/Plug-Ins/HAL/rcp_plugin.driver

kickstart:
	launchctl kickstart -k system/com.apple.audio.coreaudiod
