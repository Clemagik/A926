#!/bin/sh 	1

# update wifi soft reset function
rtwpriv wlan0 efuse_set wmap,0A,ED35
