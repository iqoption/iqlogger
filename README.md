IQ Logger
=======================

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![Build Status](https://travis-ci.org/iqoption/slack-duty-bot.svg?branch=master)](https://travis-ci.org/iqoption/iqlogger)

**IQLogger** - высокопроизводительное легковесное решение для сбора, процессинга, транспорта и балансировки логов из любых источников, включая:

 - логи своих приложений
 - логи nginx, postgresql и других стандартных приложений
 - системные логи (syslog, journal, dmesg и пр.)
 - логи docker - контейнеров

В качестве источника логов могут быть использованы:

 - файлы (поддерживаются многострочные записи и ротация файлов)
 - TCP/UDP потоки в формате GELF
 - TCP/UDP потоки в формате JSON
 - Системный журнал 

Для процессинга логов может быть использован высокопроизводительный JavaScript-процессор на основе V8.