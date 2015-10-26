/*
 * ObjectTracker.h
 *
 *  Created on: Sep 3, 2015
 *      Author: leon
 */

#pragma once

#include "TrackerConfig.h"
#include "AbstractTracker.h"

#include <rsc/logging/Logger.h>
#include <vector>

namespace objecttracker {

class ObjectTracker {
private:
    std::vector<TrackerConfig> trackerConfigs;
    std::vector<AbstractTracker::Ptr> trackerInstances;
    std::vector<AbstractTrackerBuilder::Ptr> builders;
    const rsc::logging::LoggerPtr logger;
public:
    ObjectTracker();
    virtual ~ObjectTracker();

    bool readParams(int argc, char **argv);
    void initTrackers();
    void shutdown();

private:

    bool readConfigFile(const std::string &filename);
};

} /* namespace objecttracker */
