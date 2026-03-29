# Dockerfile - build MS4000 firmware.bin
# Author: (mon7gomery)

FROM debian:stable-20260316-slim
WORKDIR /usr/local/ms4000-builder

# install dependencies
RUN apt update
RUN apt install -y --no-install-recommends --no-install-suggests make git python3 python3-setuptools python3-pip protobuf-compiler npm build-essential

# copy current worktree to docker image
COPY ./ ./
 
# pip
# --break-system-packages
RUN pip install --root-user-action warn --break-system-packages -r firmware/requirements.txt

# npm
RUN npm install -g protoc-gen-js

# build proto and firmware
RUN cd firmware/ && \
    make proto && \
    platformio run

# build webapp
RUN cd web/app && \
    npm i && \
    npm run build && \
    ls build

# clean working directory
RUN rm -rf ./*

