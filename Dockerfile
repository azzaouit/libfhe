# Download base image
FROM ubuntu:latest

# Disable Prompt During Packages Installation
ARG DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt update
RUN apt upgrade -y
RUN apt install -y build-essential cmake valgrind
RUN apt install -y libomp-dev libgmp-dev libmpfr-dev libmpc-dev
RUN apt install -y python3 python3-pip python-is-python3

COPY . $HOME/libfhe
ENTRYPOINT /bin/bash
