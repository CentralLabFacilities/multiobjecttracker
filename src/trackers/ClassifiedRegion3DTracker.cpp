/*
 * ClassifiedRegion3DTracker.cpp
 *
 *  Created on: Sep 3, 2015
 *      Author: leon
 */

#include <ObjectTracker/trackers/ClassifiedRegion3DTracker.h>
#include <ObjectTracker/util/Utilities.h>

#include <bayes_tracking/multitracker.h>
#include <bayes_tracking/models.h>
#include <bayes_tracking/ekfilter.h>
#include <bayes_tracking/ukfilter.h>
#include <bayes_tracking/pfilter.h>
#include <bayes_tracking/trackwin.h>

using namespace MTRK;
using namespace Models;

namespace objecttracker {

ClassifiedRegion3DTracker::ClassifiedRegion3DTracker(TrackerConfig config) :
        AbstractTrackerRst<DataType, STATE_DIMENSIONS>(OBSERVATION_DIMENSIONS, config) {

}

ClassifiedRegion3DTracker::~ClassifiedRegion3DTracker() {
}

Observations ClassifiedRegion3DTracker::makeObservations(DataPtr data) {

    Observations obsvs;
    for (int i = 0; i < data->region_size(); i++) {

        Observation obsv(OBSERVATION_DIMENSIONS);
        obsv[0] = data->region(i).region().region().transformation().translation().x();
        obsv[1] = data->region(i).region().region().transformation().translation().y();
        obsv[2] = data->region(i).region().region().transformation().translation().z();

        obsvs.push_back(obsv);
    }
    return obsvs;
}

ClassifiedRegion3DTracker::DataPtr ClassifiedRegion3DTracker::makeResults(DataPtr data,
        const std::vector<Track> &tracks, const std::map<int, int> obsvMappingOut) {

    // update internal map from observations to tracks
    std::map<int, int>::const_iterator mappingIt;
    for (mappingIt = obsvMappingOut.begin(); mappingIt != obsvMappingOut.end(); ++mappingIt) {
        lastObservationsById[mappingIt->second] = data->region(mappingIt->first);
    }

    // fill return type with observations from all tracks
    DataPtr result(DataType().New());
    std::map<int, rst::tracking::TrackedClassifiedRegion3D>::const_iterator obsvIt;
    for (obsvIt = lastObservationsById.begin(); obsvIt != lastObservationsById.end(); ++obsvIt) {
        rst::tracking::TrackedClassifiedRegion3D* newRegion = result->add_region();
        newRegion->CopyFrom(obsvIt->second);
        newRegion->mutable_info()->set_id(obsvIt->first);
    }

    return result;
}

} /* namespace objecttracker */
