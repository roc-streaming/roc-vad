# Hacking guide

<!-- toc -->

- [Introduction](#introduction)
  * [Design overview](#design-overview)
  * [Persistent state](#persistent-state)
  * [Components](#components)
- [Driver internals](#driver-internals)
  * [Class diagram](#class-diagram)
  * [Threads](#threads)
  * [Synchronization](#synchronization)
- [Build system](#build-system)
  * [Dependencies](#dependencies)
  * [Build phases](#build-phases)
  * [Using ccache](#using-ccache)
  * [Linker symbols](#linker-symbols)
- [Makefile targets](#makefile-targets)
  * [Development commands](#development-commands)
  * [Debugging commands](#debugging-commands)
  * [Documentation commands](#documentation-commands)

<!-- tocstop -->

## Introduction

### Design overview

Virtual device is implemented as a plugin, a.k.a. driver, for CoreAudio sound daemon.

The driver is running in a sandbox, isolated from filesystem, but with network access. The streaming part is integrated right into the driver. The command-line tool controls the driver via gRPC over a TCP socket on localhost.

To communicate with CoreAudio HAL, Roc VAD uses [libASPL](https://github.com/gavv/libASPL) (developed by the author of these lines). You can refer to its documentation to get familiar with CoreAudio HAL concepts, which affect Roc VAD design quite a lot.

For streaming, Roc VAD uses [Roc Toolkit API](https://roc-streaming.org/toolkit/docs/api/reference.html). Refer to its documentation for more details about interfaces, endpoints, protocols, and various sender and receiver options.

### Persistent state

To keep things simple, there is no state outside the driver (no configuration files, no daemon, etc).

Since driver can be restarted at any point (because coreaudiod may restart), it should be able to restore its state. To achieve this, it stores serialized state in persistent storage provided by CoreAudio HAL. (There no access to filesystem, but there is a key-value storage that can be used by plugins.)

Implementation of saving and loading state is straightforward: since driver is entirely configurable via gRPC, we already have protobuf messages for everything needed to configure driver. Thus we can just store these serialized protobuf messages after each configuration update, and load them at driver start.

### Components

The code is split into a few top-level components:

* [`common`](common) - static library with code shared between driver and CLI tool
* [`rpc`](rpc) - gRPC spec and static library with generated stubs, also shared between driver and CLI tool
* [`driver`](driver) - macOS bundle with CoreAudio plugin
* [`tool`](tool) - CLI tool to control driver
* [`test`](test) - unit and integration tests for both driver and tool parts

## Driver internals

### Class diagram

![class diagram](docs/classes.svg)

### Threads

There are three types of threads on which driver code is executed:

* gRPC control thread(s)

    gRPC handles incoming requests on its own threads. gRPC calls are delegated to `DriverService` class.

* CoreAudio control thread(s)

    CoreAudio HAL queries and updates various properties of driver, plugin, and device objects from its own control threads. Most of these operations are handled by libASPL under the hood. The only operations handled by us are `OnStartIO()` and `OnStopIO()` methods of `RequestHandler` class.

* CoreAudio real-time I/O thread(s)

    CoreAudio HAL performs I/O on real-time threads. In the end it invoke `OnReadClientInput()` and `OnWriteMixedOutput()` methods of `RequestHandler` class. These methods should be realtime-safe, i.e. should not block on any resource that can be potentially locked by a non-realtime thread, to avoid priority inversion problems.

Note that different devices may be used from different control or I/O threads. We don't make any assumptions on this in code.

### Synchronization

Here is what state is shared between threads:

* `DeviceManager` and `LogManager` are being used from `DriverService` and implement thread-safety for it, because they can be invoked from different gRPC threads.

* `Device` is used only from `DeviceManager`. Since `DeviceManager` already serializes all work, there is no need to implement thread-safety here.

* `RequestHandler` is being used from HAL via libASPL. libASPL ensures that all calls to its methods are serialized, so there is no need to implement thread-safety there.

* `Sender` and `Receiver` are used from `Device` (i.e. from gRPC threads) and from `RequestHandler` (i.e. from HAL control and I/O threads). It means that they should be thread-safe. Furthermore, their `write()` and `read()` operations should be also realtime-safe, since they're invoked from I/O thread.

Roc Toolkit provides exactly those guarantees needed by `Sender` and `Receiver`:

* `roc_sender` and `roc_receiver` are thread-safe
* control operations like binding, connecting, or querying metrics, may be invoked from any thread
* I/O operations, i.e. writing or reading, are lock-free and are not blocked by concurrently running control operations

`Sender` and `Receiver` classes rely on this fact and don't need to implement any additional thread-safety measures, since they don't have any state except `roc_sender` and `roc_receiver` handles.

## Build system

### Dependencies

The project uses these libraries:

* [Roc Toolkit](https://github.com/roc-streaming/roc-toolkit) - network streaming
* [libASPL](https://github.com/gavv/libASPL) - virtual device support
* [gRPC](https://github.com/grpc/grpc) - control protocol for virtual device
* [BoringSSL](https://github.com/google/boringssl) - encryption (gRPC dependency)
* [CLI11](https://github.com/CLIUtils/CLI11) - command-line parsing library
* [spdlog](https://github.com/gabime/spdlog) - logging library
* [{fmt}](https://github.com/fmtlib/fmt) - formatting library
* [GoogleTest](https://github.com/google/googletest) - testing library

All dependencies listed above are downloaded automatically. Besides them, project needs some standard frameworks, build tools installed system-wide (README lists them), and Xcode or Xcode command-line tools with C++17 support.

### Build phases

The build system uses CMake. Build consists of two phases:

* "bootstrap" - build all dependencies (using ExternalProject) and exit early
* "normal build" - update dependencies, if necessary, then build the project

Bootstrap phase, enabled by `-DBOOTSTRAP=ON`, is needed to be able to use FindPackage facility for dependencies. It wouldn't be possible otherwise, because ExternalProject runs in the build stage (i.e. in make), and FindPackage runs in the configuration stage (i.e. in cmake).

When you type "make build", it will automatically check if bootstrap phase is already completed, and run it if needed, so usually you do not have to bother about it.

### Using ccache

If ccache is installed on the system, it is employed automatically.

To install it, use:

```
brew install ccache
```

If you frequently do a full clean and rebuild, it can significantly speed up your workflow.

### Linker symbols

All dependencies are build into static libraries and linked into the driver. All driver symbols, except plugin entry point, are hidden.

There are two important reasons for this:

* We can not use shared libraries for common stuff like gRPC or spdlog, because if other CoreAudio plugins will also use them, they may need different versions, and there will be a conflict.

* After statically linking these libraries, we can not allow exporting their symbols from our plugin, because, again, if other plugins will use the same libraries, symbols from our plugin may overlap with the same symbols from other plugins, and there will be a mess.

## Makefile targets

### Development commands

Build:

```
make [build]
```

Run tests:

```
make test
```

Clean build results:

```
make clean
```

Print various info about binaries, like size, imports, symbols, etc:

```
make info
```

Format code using clang-format:

```
make fmt
```

### Debugging commands

Stream driver logs from syslog (when RPC doesn't work, or e.g. to get early initialization logs):

```
make syslog
```

Show audio devices:

```
make sysprof
```

Restart coreaudiod:

```
sudo make kick
```

### Documentation commands

After modifying `.proto` file(s), you need to re-generate [RPC.md](RPC.md) document.

Install [protoc-gen-doc](https://github.com/pseudomuto/protoc-gen-doc):

```
go install github.com/pseudomuto/protoc-gen-doc/cmd/protoc-gen-doc@latest
```

Regenerate documentation:

```
make rpcmd
```

After modifying `.d2` file(s), you need to re-generate `.svg` images.

Install [d2](https://d2lang.com/):

```
go install oss.terrastruct.com/d2
```

Regenerate images:

```
make d2
```

After modifying sections in README.md or HACKING.md, you need to re-generate table of contents.

Install [markdown-toc](https://github.com/jonschlinkert/markdown-toc):

```
npm install -g markdown-toc
```

Regenerate TOC:

```
make toc
```
