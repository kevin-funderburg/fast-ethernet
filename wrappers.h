//
// Created by Kevin Funderburg on 12/10/20.
//

#ifndef WRAPPERS_H
#define WRAPPERS_H

#endif //WRAPPERS_H


int Socket(int family, int type, int protocol)
{
    int n;

    if ((n = socket(family, type, protocol)) < 0)
        error("socket error");
    return (n);
}

