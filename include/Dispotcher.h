//
// Created by asr on 26/06/18.
//

#ifndef BRO_SPOTD_DISPOTCHER_H
#define BRO_SPOTD_DISPOTCHER_H

#include "MonitoredFeature.h"
#include "iniparser/iniparser.h"
#include "colors.h"
#include <string>
#include <list>
#include <vector>
#include <map>

using namespace std;

/**
 * This class is a kind of dispatcher (with the funny pun with SPOT, the monitoring algorithm).
 * It embeds a list of features to monitor. It will dispatch input data to the correct
 * MonitoredFeature instance
 */
class Dispotcher {
private:
    /**
     * The list of all the features to monitor
     */
    list<MonitoredFeature> _features;


public:
    /**
     * COnstructor for .ini file
     * @param ini_file the configuration file
     * This file needs to have the following format
     * [feature]
     * type = (see see https://www.bro.org/sphinx/broccoli-api/broccoli_8h.html)
     * spot parameters  (see https://asiffer.github.io/libspot/tuto/)
     * An example is the following:
[default]
bro_type = 5
q = 1e-4
n_init = 1000
level = 0.99
up = true
down = false
bounded = true
max_excess = 200

[avg_pkt_size]
bro_type = 5
q = 5e-4
     */
    Dispotcher(string ini_file);

    /**
     * Internal constructor (for a test purpose)
     * @param feature_names the list of the features name
     * @param feature_types a map giving the type (BRO type) of the feature
     * @param configs a map giving the SPOT configuration to monitor the feature
     */
    Dispotcher(list<string> &feature_names, map<string, int> &feature_types,
               map<string, SpotConfig> &configs);

    /**
     *
     * @return the list of all the monitored features
     */
    list<string> names();

    /**
     *
     * @return the number of monitored features
     */
    size_t size();

    /**
     *
     * @return the beginning of the list of the monitored features
     */
    list<MonitoredFeature>::iterator begin();

    /**
     *
     * @return the end of the list of the monitored features
     */
    list<MonitoredFeature>::iterator end();

    /**
     * Get the status of the spot instance monitoring the given feature
     * @param feature the name of the monitored feature
     * @return
     */
    SpotStatus status(string feature);

    /**
     * Get the initial config of the spot instance monitoring the given feature
     * @param feature the name of the monitored feature
     * @return
     */
    SpotConfig config(string feature);

    /**
     * Reset a Spot instance
     * @param feature the monitored feature considered
     */
    void reset(string feature);

};


#endif //BRO_SPOTD_DISPOTCHER_H
