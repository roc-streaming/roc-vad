# CoreAudio plugin for Roc Toolkit

[![Build](https://github.com/roc-streaming/roc-coreaudio-plugin/workflows/build/badge.svg)](https://github.com/roc-streaming/roc-coreaudio-plugin/actions) [![Matrix chat](https://matrix.to/img/matrix-badge.svg)](https://app.element.io/#/room/#roc-streaming:matrix.org)

*Work in progress!*

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
git clone https://github.com/roc-streaming/roc-coreaudio-plugin.git
```

Go to the project directory:

```
cd roc-coreaudio-plugin
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

First, check that sound server plugin was successfully loaded:

```
$ rcp-ctl info
plugin is loaded

plugin:
  version: 0.0.1
  commit:  76ca125

client:
  version: 0.0.1
  commit:  76ca125
```

*TODO*

## Troubleshooting

*TODO*

## Interoperability

*TODO*

## Programmatic control

*TODO*

## Hacking

*TODO: bootstrap, codesign, ccache, symbols*

Contributions are welcome!

## Authors

See [here](https://github.com/roc-streaming/roc-coreaudio-plugin/graphs/contributors).

## License

Roc CoreAudio plugin is licensed under [MPL-2.0](LICENSE).

For details on Roc Toolkit licensing, see [here](https://roc-streaming.org/toolkit/docs/about_project/licensing.html).
