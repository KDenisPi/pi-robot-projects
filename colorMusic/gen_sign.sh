#!/bin/sh

signalgen -v -s 40000 -t 1 -f -w sin$1Hz_1sec.wav sin $1
