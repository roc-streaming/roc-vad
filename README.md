# Roc Virtual Audio Device for macOS

[![Build](https://github.com/roc-streaming/roc-vad/workflows/build/badge.svg)](https://github.com/roc-streaming/roc-vad/actions) [![Matrix chat](https://matrix.to/img/matrix-badge.svg)](https://app.element.io/#/room/#roc-streaming:matrix.org)

> <h2>PROJECT IS WORK IN PROGRESS!</h2>

## What is this?

This repo provides macOS **Virtual Audio Device** (VAD) for **streaming audio** to or from remote devices. It is part of [Roc Toolkit](https://github.com/roc-streaming/roc-toolkit) project and is interoperable with both Roc and third-party software.

After installing Roc VAD, you can:

* create and control one or several virtual audio devices, using a command-line tool;
* connect a virtual device to remote sender or receiver;
* configure you local apps to use a virtual device instead of real microphone or speakers.

## How it works

When you configure an app (e.g. iTunes) to use virtual device as a speaker, all sound that the app plays to the device is streamed to a remote receiver. Remote receiver could be Roc PipeWire source, or an app using Roc Toolkit library, or even a generic RTP receiver.

And vice versa, when you configure an app (e.g. Zoom) to use virtual device as a micrhophone, the sound is streamed from a remote sender to the virtual device, and the app reads the received stream.

## Features

Key features of Roc Toolkit:

* real-time streaming with guaranteed latency;
* robust work on unreliable networks like Wi-Fi, due to use of Forward Erasure Correction codes;
* CD-quality audio;
* multiple profiles for different CPU and latency requirements;
* relying on open, standard protocols, like RTP and FECFRAME;
* interopability with both Roc and third-party software.

Compatible Roc Toolkit senders and receivers include:

* [cross-platform command-line tools](https://roc-streaming.org/toolkit/docs/tools/command_line_tools.html)
* [modules for various sound servers](https://roc-streaming.org/toolkit/docs/tools/sound_server_modules.html) (PulseAudio, PipeWire)
* [C library](https://roc-streaming.org/toolkit/docs/api.html) and [bindings for other languages](https://roc-streaming.org/toolkit/docs/api/bindings.html) (Go, Java)
* [Android app](https://github.com/roc-streaming/roc-droid)

## Installation

Prerequisites:

* Install XCode Command Line Tools
* Install [Homebrew](https://brew.sh/)

Install build dependencies:

```
brew install cmake scons autoconf automake pkg-config libtool
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

*TODO*

## Troubleshooting

*TODO*

## Programmatic control

*TODO*

## Hacking

*TODO: bootstrap, codesign, ccache, symbols, libASPL, gRPC*

Contributions are welcome!

## Authors

See [here](https://github.com/roc-streaming/roc-vad/graphs/contributors).

## License

Contents of the repository is licensed under [MPL-2.0](LICENSE).

For details on Roc Toolkit licensing, see [here](https://roc-streaming.org/toolkit/docs/about_project/licensing.html).
