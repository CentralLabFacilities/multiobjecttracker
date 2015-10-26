/*
 * ClassifiedRegion3DTracker.h
 *
 *  Created on: Sep 3, 2015
 *      Author: leon
 */

#pragma once

#include "../AbstractTrackerRst.h"
#include <rst/tracking/TrackedPolygonalPatch3DSet.pb.h>

#define STATE_DIMENSIONS 6
#define OBSERVATION_DIMENSIONS 3

namespace objecttracker {

class PolygonalPatch3DTracker: public
        AbstractTrackerRst<rst::tracking::TrackedPolygonalPatch3DSet, STATE_DIMENSIONS> {
private:
    std::map<int, rst::tracking::TrackedPolygonalPatch3D> lastObservationsById;
public:
    typedef boost::shared_ptr<PolygonalPatch3DTracker> Ptr;
    PolygonalPatch3DTracker(TrackerConfig config);
    virtual ~PolygonalPatch3DTracker();

    virtual Observations makeObservations(DataPtr data);
    virtual DataPtr makeResults(DataPtr data, const std::vector<Track> &tracks, const std::map<int, int> obsvMappingOut);

    CREATE_BUILDER_NESTED(PolygonalPatch3DTracker);
};

} /* namespace objecttracker */
