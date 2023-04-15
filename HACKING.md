# Hacking guide

## Makefile targets

Build:

```
make [build]
```

Clean build results:

```
make clean
```

Print various info about binaries, like size, imports, symbols, etc:

```
make info
```

Format code:

```
make fmt
```

## General overview

Virtual device is implemented as a plugin, a.k.a. driver, for CoreAudio sound daemon. The most part of the driver job is provided by [libASPL](https://github.com/gavv/libASPL) (developed by the author of these lines).

The driver is running in a sandbox, isolated from filesystem, but with network access. The streaming part is integrated right into the driver. The command-line tool controls the driver via gRPC over a TCP socket on localhost.

## Dependencies

The project uses these libraries:

* [Roc Toolkit](https://github.com/roc-streaming/roc-toolkit) - network streaming
* [libASPL](https://github.com/gavv/libASPL) - virtual device support
* [gRPC](https://github.com/grpc/grpc) - control protocol for virtual device
* [CLI11](https://github.com/CLIUtils/CLI11) - command-line parsing library
* [spdlog](https://github.com/gabime/spdlog) - logging library
* [{fmt}](https://github.com/fmtlib/fmt) - formatting library

All dependencies listed above are shipped as submodules. Besides them, project needs some standard frameworks, build tools installed system-wide, and Xcode or Xcode command-line tools with C++17 support.

## Build system

The build system uses CMake. Build consists of two phases:

* "bootstrap" - build all dependencies (using ExternalProject) and exit early
* "normal build" - update dependencies, if necessary, then build the project

Bootstrap phase, enabled by `-DBOOTSTRAP=ON`, is needed to be able to use FindPackage facility for dependencies. It wouldn't be possible otherwise, because ExternalProject runs in the build stage (i.e. in make), and FindPackage runs in the configuration stage (i.e. in cmake).

When you type "make build", it will automatically check if bootstrap phase is already completed, and run it if needed, so usually you do not have to bother about it.

## Ccache

If ccache is installed on the system, it is employed automatically.

To install it, use:

```
brew install ccache
```

If you frequently do a full clean and rebuild, it can significantly speed up your workflow.

## Linker symbols

All dependencies are build into static libraries and linked into the driver. All driver symbols, except plugin entry point, are hidden.

There are two important reasons for this:

* We can not use shared libraries for common stuff like gRPC or spdlog, because if other CoreAudio plugins will also use them, they may need different versions, and there will be a conflict.

* After statically linking these libraries, we can not allow exporting their symbols from our plugin, because, again, if other plugins will use the same libraries, symbols from our plugin may overlap with the same symbols from other plugins, and there will be a mess.

## Code signing

*TODO*

## Socket address

*TODO*
