/*
 * Utilities.h
 *
 *  Created on: Sep 6, 2015
 *      Author: leon
 */

#pragma once

#include <sys/time.h>
#include <stdio.h>

namespace objecttracker {

class Utilities {
public:
    Utilities();
    virtual ~Utilities();

    static double getTimeSeconds() {
      timeval t;
      gettimeofday(&t,NULL);
      return (double)t.tv_sec + (double)t.tv_usec/1000000.0;
    }
};

} /* namespace objecttracker */
