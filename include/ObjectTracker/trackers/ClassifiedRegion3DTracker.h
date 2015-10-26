/*
 * ClassifiedRegion3DTracker.h
 *
 *  Created on: Sep 3, 2015
 *      Author: leon
 */

#pragma once

#include "../AbstractTrackerRst.h"
#include <rst/tracking/TrackedClassifiedRegions3D.pb.h>

#define STATE_DIMENSIONS 6
#define OBSERVATION_DIMENSIONS 3

namespace objecttracker {

class ClassifiedRegion3DTracker: public
        AbstractTrackerRst<rst::tracking::TrackedClassifiedRegions3D, STATE_DIMENSIONS> {
private:
    std::map<int, rst::tracking::TrackedClassifiedRegion3D> lastObservationsById;
public:
    typedef boost::shared_ptr<ClassifiedRegion3DTracker> Ptr;
    ClassifiedRegion3DTracker(TrackerConfig config);
    virtual ~ClassifiedRegion3DTracker();

    virtual Observations makeObservations(DataPtr data);
    virtual DataPtr makeResults(DataPtr data, const std::vector<Track> &tracks, const std::map<int, int> obsvMappingOut);

    CREATE_BUILDER_NESTED(ClassifiedRegion3DTracker);
};

} /* namespace objecttracker */
