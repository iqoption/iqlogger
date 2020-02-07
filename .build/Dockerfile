ARG TAG=ubuntu18

FROM neogenie/iqlogger_builder:$TAG

ARG VERSION
ARG TAG
ARG ARCH
ARG UBUNTU

ARG CI_PIPELINE_ID
ARG CI_COMMIT_REF_NAME
ARG CI_COMMIT_MESSAGE
ARG CI_COMMIT_SHA
ARG CI_USER

ADD . /iqlogger
WORKDIR iqlogger

RUN .build/build-${TAG}.sh

RUN cp iqlogger .build_deb/${TAG}/usr/local/bin/
RUN bash .build_deb/create-deb.sh