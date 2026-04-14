# Dockerfile - build MS4000 firmware.bin
# Author: (mon7gomery)
FROM debian:stable-slim

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
    make git rsync curl ca-certificates protobuf-compiler build-essential \
 && rm -rf /var/lib/apt/lists/*

# Switch to non-root user early
USER builder

# Install mise
RUN curl -fsSL https://mise.run | bash
ENV PATH="/home/builder/.local/bin:/home/builder/.local/share/mise/shims:$PATH"

# install toolchains using mise
COPY --chown=builder:buildergroup mise.toml ./
RUN mise trust && mise install && mise reshim

# install python dependecies
COPY --chown=$UID:$GID ./firmware/requirements.txt ./firmware/requirements.txt
RUN python -m pip install --upgrade pip
RUN python -m pip install -r ./firmware/requirements.txt

# mise runtime PATH setup
ENV PATH="/home/builder/.local/share/mise/installs/python/latest/bin:$PATH"

# Now copy source code (this layer will invalidate most often) ===
COPY --chown=$UID:$GID ./ ./

# Build webapp
RUN cd web/app && make deps && make

# Build steps (these will only re-run if the code above them changed)
RUN cd firmware/ && make 

# Optional: clean up as root if needed (but consider leaving build artifacts if you mount volumes)
#USER root
# RUN rm -rf ./*   # <-- usually not needed when you mount -v $(PWD):/home/builder/workspace
