# This file is a Bro script aimed to declare events sent by bro-spot
# Actually, bro-spot send only anomalies
# Put it in /usr/share/bro/policy/custom
# Declare it /usr/share/bro/site/local.bro
# Append "@load custom/spot-anomaly"
global spot_anomaly: event(req_id: int, feature: string, proba: double);


module SpotAnomaly;
export {
    # Create an ID for our new stream. By convention, this is
    # called "LOG".
    redef enum Log::ID += { LOG };

    # Define the record type that will contain the data to log.
    type Info: record {
        ts: time            &log;
        id: int             &log;
        feature: string     &log;
        probability: double &log;
    };
}


# This event is handled at a priority higher than zero so that if
# users modify this stream in another script, they can do so at the
# default priority of zero.
event bro_init() &priority=10
{
    # Create the stream. This adds a default filter automatically.
    Log::create_stream(SpotAnomaly::LOG, [$columns=Info, $path="spot-anomaly"]);
}


event spot_anomaly(req_id: int, feature: string, proba: double)
{
    local rec = SpotAnomaly::Info(  $ts = network_time(),
                                    $id = req_id,
                                    $feature = feature,
                                    $probability = proba);
    Log::write(SpotAnomaly::LOG, rec);
}