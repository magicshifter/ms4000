# Dockerfile - build MS4000 firmware.bin
# Author: (mon7gomery)
FROM debian:stable-20260316-slim

WORKDIR /home/builder/workspace

# Python i/o should be unbuffered for tooling to work properly:
ENV PYTHONUNBUFFERED=1

# Fallback default - should be set with --build-arg UID=/GID= in the Makefile
ARG UID=1000
ARG GID=1000

# Create user/group
RUN groupadd -g $GID buildergroup \
 && useradd -u $UID -g buildergroup -m builder \
 && groupadd -f dialout \
 && usermod -aG dialout builder \
 && chown builder:buildergroup /home/builder

# Install system dependencies
RUN apt-get update && apt-get install -y --no-install-recommends --no-install-suggests \
    make git rsync python3 python3-setuptools python3-pip protobuf-compiler npm build-essential \
 && rm -rf /var/lib/apt/lists/*

# npm global
RUN npm install -g protoc-gen-js

# === Python dependencies - copy ONLY requirements first ===
COPY ./firmware/requirements.txt ./firmware/requirements.txt
RUN pip install --root-user-action warn --break-system-packages -r firmware/requirements.txt

# Switch to non-root user early
USER builder

# === Now copy source code (this layer will invalidate most often) ===
COPY --chown=$UID:$UID ./ ./

# Build steps (these will only re-run if the code above them changed)
RUN cd firmware/ && make proto && platformio run

# Build webapp
RUN cd web/app && make deps && make

# Optional: clean up as root if needed (but consider leaving build artifacts if you mount volumes)
USER root
# RUN rm -rf ./*   # <-- usually not needed when you mount -v $(PWD):/home/builder/workspace
