/*
 * PositionTracker.h
 *
 *  Created on: Sep 3, 2015
 *      Author: leon
 */

#pragma once

#include "../AbstractTrackerRst.h"
#include <rst/tracking/TrackedPositions3DFloat.pb.h>

#define STATE_DIMENSIONS 6
#define OBSERVATION_DIMENSIONS 3

namespace objecttracker {

class PositionTracker: public
        AbstractTrackerRst<rst::tracking::TrackedPositions3DFloat, STATE_DIMENSIONS> {
private:
    std::map<int, rst::tracking::TrackedPosition3DFloat> lastObservationsById;
public:
    typedef boost::shared_ptr<PositionTracker> Ptr;
    PositionTracker(TrackerConfig config);
    virtual ~PositionTracker();

    virtual Observations makeObservations(DataPtr data);
    virtual DataPtr makeResults(DataPtr data, const std::vector<Track> &tracks, const std::map<int, int> obsvMappingOut);

    CREATE_BUILDER_NESTED(PositionTracker);
};

} /* namespace objecttracker */
