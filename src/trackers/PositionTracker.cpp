/*
 * PositionTracker.cpp
 *
 *  Created on: Sep 3, 2015
 *      Author: leon
 */

#include <ObjectTracker/trackers/PositionTracker.h>
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

PositionTracker::PositionTracker(TrackerConfig config) :
        AbstractTrackerRst<DataType, STATE_DIMENSIONS>(OBSERVATION_DIMENSIONS, config) {

}

PositionTracker::~PositionTracker() {
}

Observations PositionTracker::makeObservations(DataPtr data) {

    Observations obsvs;
    for (int i = 0; i < data->position_size(); i++) {

        Observation obsv(OBSERVATION_DIMENSIONS);
        obsv[0] = data->position(i).position().x();
        obsv[1] = data->position(i).position().y();
        obsv[2] = data->position(i).position().z();

        obsvs.push_back(obsv);
    }
    return obsvs;
}

PositionTracker::DataPtr PositionTracker::makeResults(DataPtr data,
        const std::vector<Track> &tracks, const std::map<int, int> obsvMappingOut) {

    // update internal map from observations to tracks
    std::map<int, int>::const_iterator mappingIt;
    for (mappingIt = obsvMappingOut.begin(); mappingIt != obsvMappingOut.end(); ++mappingIt) {
        lastObservationsById[mappingIt->second] = data->position(mappingIt->first);
    }

    // fill return type with observations from all tracks
    DataPtr result(DataType().New());
    std::map<int, rst::tracking::TrackedPosition3DFloat>::const_iterator obsvIt;
    for (obsvIt = lastObservationsById.begin(); obsvIt != lastObservationsById.end(); ++obsvIt) {
        rst::tracking::TrackedPosition3DFloat* newPosition = result->add_position();
        newPosition->CopyFrom(obsvIt->second);
        newPosition->mutable_info()->set_id(obsvIt->first);
    }

    return result;
}

} /* namespace objecttracker */
