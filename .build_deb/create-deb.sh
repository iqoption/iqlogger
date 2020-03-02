#!/bin/bash

rm .build_deb/${TAG}/var/log/iqlogger/.gitkeep
rm .build_deb/${TAG}/var/lib/iqlogger/.gitkeep
rm .build_deb/${TAG}/usr/local/bin/.gitkeep
cd .build_deb/${TAG}
fpm -s dir -t deb -a ${ARCH} -n iqlogger -v ${VERSION}-${UBUNTU} -m neogenie@yandex.ru \
    -d libtbb2 -d zlib1g \
    --pre-install ../pre-install.sh \
    --after-install ../post-install.sh \
    --vendor neogenie@yandex.ru \
    --config-files etc/iqlogger \
    etc/ usr/ var/
