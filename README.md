# Fast-Ethernet

Author: Kevin Funderburg


<!-- TOC depthFrom:2 -->

- [About](#about)
- [Usage](#usage)
    - [Make commands](#make-commands)
    - [Examples](#examples)
- [Licensing & Thanks](#licensing--thanks)
- [Changelog](#changelog)

<!-- /TOC -->


## About
The goal of this project is to gain a fundamental understanding of process of multiplexing in a TCP client/server environment.

## Usage

### Make commands

```shell
make - compile and create executables
make clean - reset executables and object files
```

Once compiled, begin the server with `./src/csp` then the client with `./station [0-9]`.

A station process is then created with the name given as the argument (a number 0-9). Once it is running, press `return` to simulate sending the requests to the CSP and have the CSP respond.

At this point you can follow the commands as listed below.

### Examples
```shell
./obj/csp
./obj/station 0 # spawn instnance with number 0
./obj/station 1 # spawn instnance with number 1
```


## Licensing & Thanks

This project is released under the [MIT License][mit].

## Changelog

- v1.0.0

[mit]: ./LICENSE.txt
