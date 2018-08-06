//
// Created by asr on 18/04/18.
//

#ifndef BRO_SPOTD_BROCONNECTION_H
#define BRO_SPOTD_BROCONNECTION_H


#include <string>
#include <broccoli.h>
#include <iostream>
#include <iomanip>
#include <thread>
#include "colors.h"
#include <exception>

using namespace std;

/**
 * This class aims to alert when the pointer to the bro connection (C broccoli API) has
 * not been well initialized (nullptr)
 */
class bro_init_failure : public exception {
public:
    const char *what() const throw();
};


/**
 * This class aims to connect to the Bro Server (and then run the processing of input data)
 */
class BroConnection {
private:
    /**
     * IP of the Bro controller
     */
    string _ip;
    /**
     * Port of the Bro controller
     */
    int _port;
    /**
     * Pointer to the Bro connection (main object from the broccoli C API)
     */
    BroConn *_bc;

public:
    /**
     *
     * @param ip IP of the Bro controller
     * @param port Port of the Bro controller
     * @param flags Behavior for the connection. Possible values ar ethe following:
     *      BRO_CFLAG_NONE          Nothing special
     *      BRO_CFLAG_RECONNECT     Attempt transparent reconnects.
     *      BRO_CFLAG_ALWAYS_QUEUE  Queue events sent while disconnected.
     *      BRO_CFLAG_SHAREABLE     DO NOT USE – no longer supported
     *      BRO_CFLAG_DONTCACHE     Ask peer not to use I/O cache (default)
     *      BRO_CFLAG_YIELD         Process just one event at a time.
     *      BRO_CFLAG_CACHE         Ask peer to use I/O cache.
     * @throw bro_init_failure when the C pointer (see broccoli API) is NULL after initialization
     */
    BroConnection(string ip = "127.0.0.1", int port = 47760, int flags = BRO_CFLAG_NONE);

    /**
     * Destructor (free the broccoli pointer)
     */
    ~BroConnection();

    /**
     * Launch the connection
     * @param wait If true, try until connection is established
     * @return true if connected
     */
    bool connect(bool wait = true);

    /**
     * Add an event to monitor
     * @param event_name name fo the vent (the same as in the Bro script)
     * @param callback the function which will process the input
     * @param user_data extra data
     */
    void add_event(string event_name, BroEventFunc callback, void *user_data = NULL);

    /**
     * Send an event
     * @param event pointer to BroEvent
     */
    void send_event(BroEvent *event);

    /**
     *
     * @return true if connected
     */
    bool is_connected();

    /**
     *
     * @return the ip of the bro Controller
     */
    string get_ip();

    /**
     *
     * @return the port of the Bro controller
     */
    int get_port();

    /**
     * Check if the broccoli connector is OK
     * @return
     */
    operator bool() const;

    /**
     * Start to process input sent by Bro events
     */
    void run();

    /**
     *
     * @return A new thread running the 'run' method
     */
    thread silent_run();


};


#endif //BRO_SPOTD_BROCONNECTION_H
