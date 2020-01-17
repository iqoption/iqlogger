FROM ubuntu:latest

RUN apt-get update && apt-get upgrade -y && apt-get install --no-install-recommends --no-install-suggests -y curl libtbb2 zlib1g \
  && rm -rf /var/lib/apt/lists/* /var/cache/*

COPY ./ /

RUN rm /usr/local/bin/.gitkeep
RUN rm /var/log/iqlogger/.gitkeep
RUN rm /var/lib/iqlogger/.gitkeep

RUN ln -sf /dev/stdout /var/log/iqlogger/iqlogger.log

STOPSIGNAL SIGTERM

EXPOSE 12201 12202 8888

CMD ["iqlogger", "--config=/etc/iqlogger/iqlogger.json"]
