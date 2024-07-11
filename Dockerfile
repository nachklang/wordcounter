FROM ubuntu:23.04
ARG UNAME=dockeruser
ARG UID=1000
ARG GID=1000
RUN groupadd -g $GID -o $UNAME
RUN useradd -m -u $UID -g $GID -o -s /bin/bash $UNAME

USER root

RUN apt-get update && apt-get install -y \
        cmake \
        git \
        libprotobuf-dev \
        clang-tidy \
        clang \
        clang-tools \
        llvm \
        build-essential \
        && apt-get clean \
        && apt-get autoremove


RUN if [ -z "$UID" ]; then echo 'Environment variable $UID must be specified.'; exit 1; fi

RUN usermod -u $UID dockeruser

USER $UNAME

CMD ls -l