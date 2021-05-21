#!/usr/bin/env bash

avrdude -c usbasp -p t45 -U flash:w:cmake-build-debug/automatic-usb-mains-switch.hex
