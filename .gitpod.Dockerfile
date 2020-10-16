FROM gitpod/workspace-full

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
  asciinema \
  tmux \
  cppcheck \
  lcov \
  ninja-build \
  libbenchmark-dev \
  libboost-dev \
  libfmt-dev \
  libspdlog-dev \
  wget

RUN mkdir -p /workspace/data \
    && chown -R gitpod:gitpod /workspace/data
  
RUN apt-get clean && rm -rf /var/cache/apt/* && rm -rf /var/lib/apt/lists/* && rm -rf /tmp/*
