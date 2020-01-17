#!/bin/sh

chmod 755 /usr/local/bin/iqlogger
chmod 755 /var/log/iqlogger
chmod 755 /var/lib/iqlogger
chmod 644 /etc/iqlogger/iqlogger.json
chmod 644 /etc/iqlogger/processor.js
chmod 644 /etc/systemd/system/iqlogger.service

chown -R iqlogger:adm /var/log/iqlogger
chown -R iqlogger:adm /var/lib/iqlogger

systemctl daemon-reload
systemctl enable iqlogger
service iqlogger start

exit 0