#!/bin/sh

useradd -g adm -r -m -d /usr/share/iqlogger -s /bin/false iqlogger || exit 0
