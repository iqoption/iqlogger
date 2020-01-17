#!/bin/bash

VERSION=`cat version`

export IQLOGGER_STATIC_GLIBC=1
cmake .

CNT=$(which nproc > /dev/null && expr $(nproc --all) / 2 || echo 4)
if [ ${CNT} -lt 4 ] ; then CNT=4; fi
make -j ${CNT}
RET=$?

exit $RET