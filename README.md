# Fast-Ethernet

Author: Kevin Funderburg


<!-- TOC depthFrom:2 -->

- [About](#about)
- [Usage](#usage)
    - [Make commands](#make-commands)
    - [Command line structure:](#command-line-structure)
    - [Examples](#examples)
- [Licensing & Thanks](#licensing--thanks)
- [Changelog](#changelog)

<!-- /TOC -->


## About
The goal of this project is to gain a fundamental understanding of the interactions between clients and servers via TCP. This is shown through development of a school database system, where clients can make requests to a server, the server then processes the requests and returns the results.

## Usage

### Make commands

```shell
make - compile and create executables
make clean - reset executables and object files
```

Once compiled, begin the server with `./src/csp` then the client with `./station [0-9]`.

The client will then show:

    What would you like to do? (enter h for help) :

At this point you can follow the commands as listed below.

### Command line structure:

```


a add
    adds a student to the database with the following parameters:
    [student ID] [First Name] [Last Name] [Grade]
da display_all
    display all student data in database
s showscores
    show all students with a grade higher than [grade]
d delete
    delete a student from data base with student ID = [student ID]
e exit
    exit the application
h help
    outputs usage listing

```

### Examples
```shell
./obj/station 1 # this will create an instance of a station with the number 1
```


## Licensing & Thanks

This project is released under the [MIT License][mit].

## Changelog

- v1.0.0

[mit]: ./LICENSE.txt
