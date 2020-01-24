#!/usr/bin/env bash

rsync -c ${SYSROOT}/boot/bfxloader  ${INITRD_DIR}/boot/bfxloader
rsync -c ${SYSROOT}/boot/nxkernel64 ${INITRD_DIR}/boot/nxkernel64
rsync -c utils/bfx_kernel_params    ${INITRD_DIR}/boot/bfx_kernel_params
rsync -c utils/bootboot_config      ${INITRD_DIR}/sys/config

function plural() {
	if [ $2 -eq 1 -o $2 -eq -1 ]; then
		echo ${1}
	else
		echo ${1}s
	fi
}

# check if any of the files in the initrd folder are newer than the output:
NUM_FILES=$(find ${INITRD_DIR} -newer ${INITRD}.gz -type f | wc -l | xargs)
if [ ! -f ${INITRD}.gz ]; then
	NUM_FILES=1
fi

if [ ${NUM_FILES} -gt 0 ]; then
	echo "# initrd: updating ${NUM_FILES} $(plural file ${NUM_FILES})"
	pushd ${INITRD_DIR} > /dev/null
		tar -cf ${INITRD} *
		gzip -cf9 ${INITRD} > ${INITRD}.gz
	popd > /dev/null
fi
