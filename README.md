# fuji-iot

## What is this about?

I have Fuji Electric AC units at home that I wanted to become integrated with my smart home network. Unfortunatelly, those are old-school devices with IR and wired remotes and no WiFi. 

## This repository & interfacing

This package contains a simple daemon that will convert do serial to RPC conversion of the AC unit. In order to use it, you will need a device capable of handling UART @ 500bps (author of the code used Raspberry Pi Zero). Release package contains pre-compiled ARM package suitable to use with Raspbian.

## Compatibility

This software was tested against RSG09LUCA and RSG12LUCA and should interface well with Fuji Electric/Fujitsu Air conditioners that are using UTY-RNNUM, UTY-RNNXM  or similar controllers.
