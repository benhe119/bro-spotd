//
// Created by asr on 27/06/18.
//

#ifndef BRO_SPOTD_MONITOREDFEATURE_H
#define BRO_SPOTD_MONITOREDFEATURE_H

#include "spot.h"
#include "broccoli.h"

/**
 * This object aim to process a feature. It especially embeds the spot instance which checks for
 * abnormal values.
 */
class MonitoredFeature {
private:
    /**
     * The Spot instance which monitors the feature
     */
    Spot _spot;

public:
    /**
     * The name of the feature (this is the same name as in the bro script)
     */
    const string _name;
    /**
     * Broccoli type of the feature
     * Broccoli needs to know the type of the feature (even if in the end it will be converted
     * into a double).
     */
    int _bro_type;

    /**
     * Constructor
     * @param name The name of the feature
     * @param type The Bro type of the feature
     * @param config The config for Spot
     */
    MonitoredFeature(string name, int type, SpotConfig config);

    /**
     * Main method which process the input data
     * @param data input data (an observation of the feature)
     * @param ev an possible event (in case of anomaly)
     * @return true if an abnormal event occurs (false otherwise)
     */
    bool process(double data, BroEvent *ev = NULL);

    /**
     * Get the status of the underlying spot instance
     * @return the status of the Spot instance
     */
    SpotStatus status();

    /**
     * Get the config of the underlying spot instance
     * @return the config of the Spot instance
     */
    SpotConfig config();
};

#endif //BRO_SPOTD_MONITOREDFEATURE_H
