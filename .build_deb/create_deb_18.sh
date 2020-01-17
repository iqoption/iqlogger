#!/bin/bash

VERSION=`cat version`

rm .build_deb/ubuntu-18/var/log/iqlogger/.gitkeep
rm .build_deb/ubuntu-18/var/lib/iqlogger/.gitkeep
rm .build_deb/ubuntu-18/usr/local/bin/.gitkeep
cd .build_deb/ubuntu-18
fpm -s dir -t deb -a amd64 -n iqlogger -v ${VERSION}-bionic -m sergey.tarasov@iqoption.com \
    -d libtbb2 -d zlib1g \
    --pre-install ../pre-install.sh \
    --after-install ../post-install.sh \
    --vendor noc@iqoption.com \
    --config-files etc/iqlogger \
    etc/ usr/ var/
