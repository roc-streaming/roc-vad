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

install:
	cd build && make install

clean:
	rm -rf bin build

info:
	ls -lh bin/rcp-ctl bin/rcp_plugin.driver/Contents/MacOS/rcp_plugin
	@echo
	file bin/rcp-ctl bin/rcp_plugin.driver/Contents/MacOS/rcp_plugin
	@echo
	otool -L bin/rcp-ctl
	@echo
	otool -L bin/rcp_plugin.driver/Contents/MacOS/rcp_plugin
	@echo
	nm -gU bin/rcp-ctl
	nm -gU bin/rcp_plugin.driver/Contents/MacOS/rcp_plugin

fmt:
	find -type f -name '*.[ch]pp' -not -path './3rdparty/*' -not -name '*.pb.*' \
		| xargs clang-format --verbose -i
