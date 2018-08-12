#include <iostream>
#include <fstream>
#include <signal.h>
#include <sys/stat.h>
#include <thread>
#include "BroConnection.h"
#include "Dispotcher.h"
#include "Dbuspotcher.h"
#include "cola.hpp"

using namespace std;

static int nb_events = 0;

/**
 * The callback function to treat the received events
 * 
 */
void callback(BroConn *bc, void *user_data, BroRecord *record)
{
    nb_events++;
    cout << '\r' << "Current number of observations: " << nb_events;

    auto dsp = (Dispotcher *)user_data;
    double d;
    bool anomalous;
    BroEvent *out_event = nullptr;

    auto it = dsp->begin();

    double *d_ptr;
    // Loop over the monitored features
    for (; it != dsp->end(); ++it)
    {
        // from the record we get the current value of the feature
        if (!(d_ptr = (double *)bro_record_get_named_val(record, it->_name.c_str(), &it->_bro_type)))
        {
            throw runtime_error("Error when handling a recorded value");
        }

        d = *d_ptr;
        if (d > 0.) // we take only positive values but it may be useless
        {
            // we let the Spot instance processing it
            anomalous = it->process(d, out_event);

            // if Spot considers it as an anomaly, we trigger an "spot_anomaly" event
            if (anomalous)
            {
                bro_event_send(bc, out_event);
                bro_event_free(out_event);
            }
        }
    }
}

void sigterm_handler(int signal)
{
    exit(signal);
}

void sighup_handler(int signal)
{
    exit(signal);
}

int main(int argc, const char *argv[])
{

    /* Handling signals */
    signal(SIGTERM, sigterm_handler);
    signal(SIGHUP, sighup_handler);
    signal(SIGINT, sighup_handler);

    /* Argument parser */
    cola::parser parser;

    parser.define("config", "Path to the config file")
        .alias('c')
        .with_arg<string>("/etc/bro-spotd/bro-spotd.ini");

    parser.define("bro-ip", "IP address of the Bro controller")
        .alias('i')
        .with_arg<string>("127.0.0.1");

    parser.define("bro-port", "TCP port of the Bro controller")
        .alias('p')
        .with_arg<int>(47760);

    parser.define("bro-event", "Name of the Bro event we want to catch")
        .alias('e')
        .with_arg<string>("Stats::log_stats");

    parser.parse(argc, argv);

    // config file
    string ini_file = parser.get<string>("config");

    // ip of the bro controller
    string ip = parser.get<string>("bro-ip");

    // port of the bro controller
    int port = parser.get<int>("bro-port");

    // the event catched (this event may consider a Bro record)
    string event = parser.get<string>("bro-event");

    // Dispotcher creation with the config file
    cout << YEL << "Parsing " << ini_file << END << endl;
    Dispotcher *dsp = new Dispotcher(ini_file);

    // Initialization of the connection to the Bro controller
    cout << YEL << "Initializing connection to Bro "
         << "(" << ip << ":" << port << ")";
    cout << END << endl;
    BroConnection *conn = new BroConnection(ip, port);

    // we add the desired event (to catch it)
    conn->add_event(event, (BroEventFunc)callback, dsp);
    conn->connect(true);

    // we start listening on D-BUS
    cout << YEL << "Initializing D-BUS listener " << END << endl;
    Dbuspotcher *dbs = new Dbuspotcher(dsp);

    // Listen to the BUS
    thread bus(&Dbuspotcher::listen, dbs);

    // Start handling Bro Events
    cout << YEL << "Running " << END << endl;
    conn->run();

    bus.join();
    return 0;
}