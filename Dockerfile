# Dockerfile - build MS4000 firmware.bin
# Author: (mon7gomery)

FROM debian:stable-20260316-slim
WORKDIR /home/builder/workspace

# create user
ARG UID=1000
ARG GID=1000

RUN groupadd -g $GID buildergroup \
 && useradd -u $UID -g buildergroup -m builder \
 && groupadd -f dialout \
 && usermod -aG dialout builder

RUN chown builder:buildergroup /home/builder

# install dependencies
RUN apt update
RUN apt install -y --no-install-recommends --no-install-suggests make git python3 python3-setuptools python3-pip protobuf-compiler npm build-essential

# npm
RUN npm install -g protoc-gen-js

# copy requirements.txt to docker image
COPY ./firmware/requirements.txt ./firmware/requirements.txt
 
# pip
# --break-system-packages
RUN pip install --root-user-action warn --break-system-packages -r firmware/requirements.txt

# change user
USER builder

# copy working directory to docker image
COPY --chown=$UID:$UID ./ ./

# build proto and firmware
RUN cd firmware/ && \
    make proto && \
    platformio run

# build webapp
RUN cd web/app && make deps && make

# clean working directory
USER root
#RUN rm -rf ./*

