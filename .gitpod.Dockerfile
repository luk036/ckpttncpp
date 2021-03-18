FROM gitpod/workspace-full:latest

USER root
# Install util tools.
RUN apt-get update \
 && apt-get install -y \
  apt-utils \
  sudo \
  aria2 \
  git \
  less \
  neofetch \
  neovim \
  asciinema \
  tmux \
  tty-clock \
  w3m \
  cppcheck \
  lcov \
  ninja-build \
  librange-v3-dev \
  libbenchmark-dev \
  libboost-dev \
  libfmt-dev \
  libspdlog-dev \
  wget

RUN mkdir -p /workspace/data \
    && chown -R gitpod:gitpod /workspace/data
  
RUN apt-get clean && rm -rf /var/cache/apt/* && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/*
