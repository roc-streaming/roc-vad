#! /bin/bash

msg() {
    printf '\033[1;32m%s\033[0m\n' "$*"
}

msg
msg "Running Roc Virtual Audio Device installer"
msg
msg "Downloading ..."
msg

curl -L -o /tmp/roc-vad.tar.bz2 \
     https://github.com/roc-streaming/roc-vad/releases/latest/download/roc-vad.tar.bz2

msg
msg "Extracting ..."
msg

# this will add:
#  /Library/Audio/Plug-Ins/HAL/roc_vad.driver
#  /usr/local/bin/roc-vad
tar --no-same-owner -xPmvf /tmp/roc-vad.tar.bz2
rm -f /tmp/roc-vad.tar.bz2

msg
msg "Installation complete!"
msg
msg "⚠️  PLEASE REBOOT YOUR MAC NOW!"
msg
