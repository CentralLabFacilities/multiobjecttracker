/*
 * ObjectTracker.cpp
 *
 *  Created on: Sep 3, 2015
 *      Author: leon
 */

#include <ObjectTracker/ObjectTracker.h>
#include <ObjectTracker/trackers/ClassifiedRegion3DTracker.h>
#include <ObjectTracker/trackers/PositionTracker.h>

#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace rsc::logging;

namespace objecttracker {

class ConfigException: std::exception {
private:
    std::string tracker;
    std::string param;
    std::string msg;
public:
    ConfigException(const std::string &tracker, const std::string &param, const std::string &msg =
            "") :
            tracker(tracker), param(param), msg(msg) {
    }
    virtual ~ConfigException() throw () {
    }
    virtual const char* what() const throw () {
        stringstream ss;
        if (msg.empty()) {
            ss << "Tracker \"" << tracker << "\" misses parameter \"" << param << "\"";
        } else {
            ss << "Tracker \"" << tracker << "\", parameter \"" << param << "\": " << msg;
        }
        return ss.str().c_str();
    }
};

ObjectTracker::ObjectTracker() :
        logger(Logger::getLogger("ObjectTracker")) {
    builders.push_back(
            AbstractTrackerBuilder::Ptr(
                    new ClassifiedRegion3DTracker::Builder("ClassifiedRegion3DTracker")));
    builders.push_back(
                AbstractTrackerBuilder::Ptr(
                        new PositionTracker::Builder("PositionTracker")));
}

ObjectTracker::~ObjectTracker() {
}

void operator >>(const YAML::Node& node, vector<float>& vec) {
    for (int i = 0; i < node.size(); i++) {
        vec.push_back(node[i].as<float>());
    }
}
void operator >>(const YAML::Node& node, string& str) {
    str = node.as<string>();
}
void operator >>(const YAML::Node& node, TrackerConfig& tracker) {
    if (!node["name"].IsDefined()) throw ConfigException("", "name");
    node["name"] >> tracker.name;
    if (!node["sigma"].IsDefined()) throw ConfigException(tracker.name, "sigma");
    node["sigma"] >> tracker.sigma;
    if (!node["sd"].IsDefined()) throw ConfigException(tracker.name, "sd");
    node["sd"] >> tracker.sd;
    if (!node["scope_in"].IsDefined()) throw ConfigException(tracker.name, "scope_in");
    node["scope_in"] >> tracker.scope_in;
    if (!node["scope_out"].IsDefined()) throw ConfigException(tracker.name, "scope_out");
    node["scope_out"] >> tracker.scope_out;

    if (node["association_algorithm"].IsDefined())
        node["association_algorithm"] >> tracker.association_algorithm;
    if (node["filter_type"].IsDefined())
        node["filter_type"] >> tracker.filter_type;
}

bool ObjectTracker::readParams(int argc, char **argv) {
    if (argc < 2) {
        cerr << "\nError: Missing config file parameter!" << endl;
        cout << "\nUsage:\n\t" << argv[0] << " <configfile>" << endl;
        return false;
    }
    try {
        return readConfigFile(argv[1]);
    } catch (ConfigException &e) {
        cerr << "\nError: " << e.what() << endl;
        return false;
    }
}
void ObjectTracker::initTrackers() {

    for (TrackerConfig &config : trackerConfigs) {
        bool success = false;
        for (AbstractTrackerBuilder::Ptr builder : builders) {
            if (builder->getTrackerName() == config.name) {
                RSCDEBUG(logger, "Creating tracker " << config.name);
                trackerInstances.push_back(builder->build(config));
                success = true;
            }
        }
        if (!success) {
            RSCERROR(logger, "Unknown tracker type \"" << config.name << "\"");
        }
    }

    cout << "\n  Object Tracker is running" << endl;

}
void ObjectTracker::shutdown() {
    RSCDEBUG(logger, "shutdown");
}

bool ObjectTracker::readConfigFile(const string &filename) {

    YAML::Node doc = YAML::LoadFile(filename);

    const YAML::Node& tracker_list = doc["tracker_list"];

    trackerConfigs.clear();

    for (unsigned i = 0; i < tracker_list.size(); i++) {
        TrackerConfig config;
        tracker_list[i] >> config;
        trackerConfigs.push_back(config);
    }
    return true;
}

} /* namespace objecttracker */
