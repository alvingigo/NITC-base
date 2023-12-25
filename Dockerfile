FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y libc6-dev vim git nano make gcc tar wget build-essential libreadline-dev gdb


RUN useradd -m nitcbase
USER nitcbase

RUN cd /Users/gigochainyard/Desktop/alvin/dbms_lab/nitcbase  \
    && wget https://raw.githubusercontent.com/nitcbase/nitcbase-bootstrap/main/setup.sh \
    && chmod +x setup.sh \
    && mkdir NITCbase

WORKDIR /Users/gigochainyard/Desktop/alvin/dbms_lab/nitcbase/NITCbase