/*
 * TrackerConfig.h
 *
 *  Created on: Sep 5, 2015
 *      Author: leon
 */

#pragma once

#include <string>
#include <vector>

namespace objecttracker {

struct TrackerConfig {
public:
    std::string filter_type;
    std::string association_algorithm;

    std::string name;

    std::string scope_in;
    std::string scope_out;

    std::vector<float> sigma; //5.0
    std::vector<float> sd; //0.5

    TrackerConfig() {
    }
};
}

