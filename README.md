# ckpttncpp

Circuit Partitioning C++ Code

[![Gitpod Ready-to-Code](https://img.shields.io/badge/Gitpod-Ready--to--Code-blue?logo=gitpod)](https://gitpod.io/#https://github.com/luk036/ckpttncpp)
[![Build Status](https://travis-ci.com/luk036/ckpttncpp.svg?branch=master)](https://travis-ci.com/luk036/ckpttncpp)
[![Documentation Status](https://readthedocs.org/projects/ckpttncpp/badge/?version=latest)](https://ckpttncpp.readthedocs.io/en/latest/?badge=latest)
[![codecov](https://codecov.io/gh/luk036/ckpttncpp/branch/master/graph/badge.svg)](https://codecov.io/gh/luk036/ckpttncpp)
[![CodeFactor](https://www.codefactor.io/repository/github/luk036/ckpttncpp/badge)](https://www.codefactor.io/repository/github/luk036/ckpttncpp)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/63f43fb5c7034f4d863c5a86dd0cba1e)](https://app.codacy.com/app/luk036/ckpttncpp?utm_source=github.com&utm_medium=referral&utm_content=luk036/ckpttncpp&utm_campaign=Badge_Grade_Dashboard)
[![BCH compliance](https://bettercodehub.com/edge/badge/luk036/ckpttncpp?branch=master)](https://bettercodehub.com/)
[![Documentation](https://img.shields.io/badge/Documentation-latest-blue.svg)](https://luk036.github.io/doc/ckpttncpp/index.html)

## Highlights

-   Direct port from [Python version](https://github.com/luk036/ckpttnpy)
-   Work for C++20/17.
-   Support multi-level bi-partitioning and K-way partitioning
-   Special handle two-pin nets (and three-pin nets).

## Installation and Run

To run in gitpod.io:

    ./envconfig.sh  # first time when gitpod image is built

To build with Ninja:

    mkdir build && cd build
    cmake -GNinja ..
    ninja all

To run CTest:

    ninja test

Limits:

    Maximum #partitions = 255
