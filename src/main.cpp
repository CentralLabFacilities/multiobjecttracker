/*
 * main.cpp
 *
 *  Created on: Sep 3, 2015
 *      Author: leon
 */

#include <ObjectTracker/ObjectTracker.h>

#include <rsc/misc/SignalWaiter.h>

using namespace objecttracker;
using namespace std;

bool running = true;

int main(int argc, char **argv) {

    rsc::misc::initSignalWaiter();

    ObjectTracker tracker;
    if (!tracker.readParams(argc, argv)) {
        return 1;
    }

    int exit_code;
    try {
        tracker.initTrackers();

        exit_code = rsc::misc::suggestedExitCode(rsc::misc::waitForSignal());

    } catch(ModelException &e) {
        cout << "Error creating trackers: " << e.what() << endl;
    }

    tracker.shutdown();

    return exit_code;
}


