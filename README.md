# Roc Virtual Audio Device for macOS [WIP]

[![Build](https://github.com/roc-streaming/roc-vad/workflows/build/badge.svg)](https://github.com/roc-streaming/roc-vad/actions) [![GitHub release](https://img.shields.io/github/release/roc-streaming/roc-vad.svg)](https://github.com/roc-streaming/roc-vad/releases) [![Matrix chat](https://matrix.to/img/matrix-badge.svg)](https://app.element.io/#/room/#roc-streaming:matrix.org)

> <h2>PROJECT IS WORK IN PROGRESS!</h2>

## What is this?

This repo provides macOS **Virtual Audio Device** (VAD) for **streaming audio** to or from remote devices. It is part of [Roc Toolkit](https://github.com/roc-streaming/roc-toolkit) project and is interoperable with both Roc and third-party software.

After installing Roc VAD, you can:

* create and control one or several virtual audio devices, using a command-line tool;
* connect a virtual device to remote sender or receiver;
* configure you local apps to use a virtual device instead of real microphone or speakers.

## How it works

When you configure an app (e.g. iTunes) to use virtual device as a speaker, all sound that the app plays to the device is streamed to a remote receiver. Remote receiver could be Roc PipeWire source, or an app using Roc Toolkit library, or even a generic RTP receiver.

And vice versa, when you configure an app (e.g. Zoom) to use virtual device as a microphone, the sound is streamed from a remote sender to the virtual device, and the app reads the received stream.

## Features

Key features of Roc Toolkit:

* real-time streaming with guaranteed latency;
* robust work on unreliable networks like Wi-Fi, due to use of Forward Erasure Correction codes;
* CD-quality audio;
* multiple profiles for different CPU and latency requirements;
* relying on open, standard protocols, like RTP and FECFRAME;
* interoperability with both Roc and third-party software.

Compatible Roc Toolkit senders and receivers include:

* [cross-platform command-line tools](https://roc-streaming.org/toolkit/docs/tools/command_line_tools.html)
* [modules for various sound servers](https://roc-streaming.org/toolkit/docs/tools/sound_server_modules.html) (PulseAudio, PipeWire)
* [C library](https://roc-streaming.org/toolkit/docs/api.html) and [bindings for other languages](https://roc-streaming.org/toolkit/docs/api/bindings.html) (Go, Java)
* [Android app](https://github.com/roc-streaming/roc-droid)

## Install from binaries

Install driver and command-line tool:

```
sudo /bin/bash -c \
  "$(curl -fsSL https://raw.githubusercontent.com/roc-streaming/roc-vad/HEAD/install.sh)"
```

⚠️ After installation, reboot your Mac.

To **uninstall** everything, run:

```
sudo roc-vad uninstall
```

Then reboot your Mac.

## Install from sources

Prerequisites:

* Install XCode Command Line Tools
* Install [Homebrew](https://brew.sh/)

Install build dependencies:

```
brew install cmake scons autoconf automake pkg-config libtool go
```

Clone repo:

```
git clone https://github.com/roc-streaming/roc-vad.git
```

Go to the project directory:

```
cd roc-vad
```

Build:

```
make
```

Install:

```
sudo make install
```

Now, either reboot your Mac, **or** run:

```
sudo make kickstart
```

This command will restart macOS sound server. Usually it's enough, but some apps may not handle sound server restart well, so reboot is preferred.

To **uninstall** everything, run:

```
sudo make uninstall
```

## Usage

First, check that sound server driver was successfully loaded:

```
$ roc-vad info
driver is loaded

driver:
  version: 0.0.1
  commit:  76ca125

client:
  version: 0.0.1
  commit:  76ca125
```

## Troubleshooting

To enable verbose logging of the command line tool, use `-v` flag. Specify it multiple times to increase verbosity:

```
$ roc-vad -vv info
debug: opened driver bundle at "/Library/Audio/Plug-Ins/HAL/roc_vad.driver"
debug: obtained value "127.0.0.1:9712" for key "DriverSocket" from driver bundle
info: trying to connect to driver at 127.0.0.1:9712
debug: creating rpc channel
debug: sending ping command
...
```

To print logs from driver, use `logcat` command. It connects to driver via RPC and streams its logs to stdout:

```
$ roc-vad logcat
03:30:33.511 [DD] received stream_logs command
03:30:33.511 [DD] attaching log sender to dist sink
...
```

Alternatively, you can use `syslog` makefile target. It streams logs that driver sends to system log. Basically they are the same logs as produced by `logcat` command, but in slightly different format, and possibly truncated and deduplicated:

```
$ make syslog
log stream --predicate 'sender == "roc_vad"'
Filtering the log data using "process == "coreaudiod" OR sender == "roc_vad""
Timestamp                       Thread     Type        Activity             PID    TTL
2023-04-13 03:31:54.896185+0400 0xe38a     Default     0x0                  11980  0
  com.apple.audio.Core-Audio-Driver-Service: (roc_vad) [DD] received ping command
2023-04-13 03:31:54.897686+0400 0xe38a     Default     0x0                  11980  0
  com.apple.audio.Core-Audio-Driver-Service: (roc_vad) [DD] received get_info command
...
```

## Common issues

* **Can't connect to driver**

   Make sure you did not forget to reboot computer (preferably) or restart `coreaudiod` after installing Roc VAD.

* **Network connectivity**

   Ensure that sent packets reach receiver.

   When you start streaming from sender to receiver, you should see information about connected sender in receiver's logs. If you don't, it indicates problems with network connection, e.g. you use incorrect address or port, or a firewall is blocking traffic.

* **Playback shuttering**

   If you hear shuttering, try increasing target latency on the receiving side. For example, for Roc VAD receiver, try using `--target-latency=200ms` to increase latency to 200 milliseconds.

* **Decreasing latency**

   If your network allows lower latency, use `--target-latency` (receiver buffer size) option on receiving side and `--packet-length` (packet size), `--nbsrc`, and `--nbrpr` (FEC block size) on sending side.

   Each FEC block consists of `--nbsrc` packets of `--packet-length` length; `--nbrpr` should be about 2/3 of `--nbsrc`; `--target-latency` should be greater than FEC block size to allow packet repair to work.

## Programmatic control

You can control Roc VAD driver from any programming language via [gRPC](https://grpc.io/) interface.

You can to do everything you can do via command-line tool, which itself is just a wrapper for the same RPC calls. Unlike the command-line tool, gRPC interface will maintain backward compatibility on updates.

More details are available here:

* [driver_protocol.proto](rpc/driver_protocol.proto) - protocol definition
* [RPC.md](RPC.md) - generated protocol documentation

## Socket address

By default, driver starts (unauthenticated) gRPC server at `127.0.0.1:9712`.

If you want to change the address, you can edit `DriverSocket` entry in `/Library/Audio/Plug-Ins/HAL/roc_vad.driver/Contents/Info.plist` plist file. Both driver and command-line tool read address from there.

## Hacking

Contributions in any form are always welcome! You can find issues needing help using [help wanted](https://github.com/roc-streaming/roc-vad/labels/help%20wanted) and [good first issue](https://github.com/roc-streaming/roc-vad/labels/good%20first%20issue) labels.

If you would like to dig into the project internals, have a look at [HACKING.md](HACKING.md).

## Donating

If you would like to support the project financially, see details on [this page](https://roc-streaming.org/toolkit/docs/about_project/sponsors.html).

Thank you!

## Authors

See [here](https://github.com/roc-streaming/roc-vad/graphs/contributors).

## License

Contents of the repository is licensed under [MPL-2.0](LICENSE).

For details on Roc Toolkit licensing, see [here](https://roc-streaming.org/toolkit/docs/about_project/licensing.html).
