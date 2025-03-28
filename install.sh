#! /bin/bash

msg() {
    printf '\033[1;32m%s\033[0m\n' "$*"
}

die() {
    printf '\nerror: \033[1;31m%s\033[0m\n' "$*"
    exit 1
}

[ "$(id -u)" = 0 ] || die "Must be run as root"

msg
msg "Running Roc Virtual Audio Device installer"

tarball="/tmp/roc-vad.tar.bz2"

rm -rf "$tarball"

if [ -z "$1" ]; then
    msg
    msg "Downloading ..."
    msg

    url="https://github.com/roc-streaming/roc-vad/releases/latest/download/roc-vad.tar.bz2"

    curl -L -o "$tarball" "$url" || die "Failed to download $url"
else
    cp "$1" "$tarball" || die "Failed to copy $1"
fi

msg
msg "Extracting ..."
msg

# The tarball will be unpacked to:
#  /Library/Audio/Plug-Ins/HAL/roc_vad.driver
#  /usr/local/bin/roc-vad
tar --no-same-owner -xPmvf "$tarball" || die "Failed to unpack $tarball into /"
rm -f "$tarball"

msg
msg "Installation complete!"
msg
msg "⚠️  PLEASE REBOOT YOUR MAC NOW!"
msg
