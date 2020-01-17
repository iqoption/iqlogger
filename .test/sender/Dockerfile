FROM alpine:latest

RUN apk update && apk add bash

COPY wait-for-it.sh /wait-for-it.sh
RUN chmod +x /wait-for-it.sh

COPY test.sh /test.sh
RUN chmod +x /test.sh

CMD ["/wait-for-it.sh", "iqlogger:12202", "--", "./test.sh"]

