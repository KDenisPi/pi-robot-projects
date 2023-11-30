#!/bin/sh

#
# -D PCM device name check for Raspberry Pi
# --duration=15 - for test purposes only. A value of zero means infinity.
#   The default is zero, so if this option is omitted then the arecord process will run until it is killed.
# --buffer-size=0 need to check, probably it is more right for streaming
#   Buffer duration is # frames If no buffer time and no buffer size is given then the maximal allowed buffer time but not more than 500ms is set.

arecord --file-type raw --channels=1 --rate=40000 --format=FLOAT_LE -D pulse --buffer-size=50 | ./build/cmusicd