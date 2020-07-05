#/bin/bash

dir="iot/build/iotpatch"

list="
buildroot=buildroot
u-boot=u-boot
kernel-4.4.93=kernel-4.4.93
development=development
external=external
"
for i in ${list[@]}; do
	src="${i%%=*}"
	dst="${i##*=}"

	[ "${src:0:1}" == '#' ] && continue
	if [ -d "$src" ]; then
		cp -r $dir/$src/* $dst
	else
		cp -r $dir/$src $dst
	fi
done
