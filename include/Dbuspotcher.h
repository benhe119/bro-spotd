//
// Created by asr on 02/07/18.
//

#ifndef BRO_SPOTD_DBUSPOTCHER_H
#define BRO_SPOTD_DBUSPOTCHER_H

#include <dbus/dbus.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Dispotcher.h"

using namespace std;

#ifndef BRO_SPOT_BUS_NAME
#define BRO_SPOT_BUS_NAME "bro.spot"
#endif

/**
 * This class aims to control the program through dbus.
 * Initially it requests the following bus name: "bro.spot"
 * Currently, you can only get informations about the SPOT instances
 * The available interface is "bro.spot.method"
 * The available methods are:
 *      - "features" (no argument needed)
 *      - "spotstatus" (string for the name of the desired feature)
 *      - "spotconfig" (string for the name of the desired feature)
 *      - "kill" (no argument)
 *      - "reset"
 */
class Dbuspotcher
{
private:
  /**
     * Pointer to the dispotcher (SPOT dispatcher)
     */
  Dispotcher *_dsp;

  /**
     * A pointer to handle DBUS errors
     */
  DBusError *_err;

  /**
  * The pointer to the DBUS connection
  */
  DBusConnection *_conn;

  /**
  * Basic function to send a reply message
  * @param reply 
  * @param extra_info additional info in case of error
  * @throw overflow_error When data have not been sent
  */
  void _send_flush_unref(DBusMessage *reply, string extra_info = "");

  /**
     * Reply function for the features.
     * It returns (through DBUS) the monitored features.
     * @param msg the incoming DBUS message
     */
  void reply_features(DBusMessage *msg);

  /**
     * Reply function for the status of a given SPOT instance.
     * It returns (through DBUS) the status of the SPOT monitoring a given feature.
     * The returned information are the following:
     *     n        number of seen observations
     *     ex_up    total number of up excesses
     *     ex_down  total number of down excesses
     *     Nt_up    current number of up excesses
     *     Nt_down  current number of down excesses
     *     al_up    total number of up alarms
     *     al_down  total number of down alarms
     *     t_up     current transitional up threshold
     *     t_down   current transitional down threshold
     *     z_up     current up decision threshold
     *     z_down   current down decision threshold
     * @param msg the incoming DBUS message (argument: a string for the name of the feature)
     */
  void reply_spotstatus(DBusMessage *msg);

  /**
     * Reply function for the config of a given SPOT instance.
     * It returns (through DBUS) the config of the SPOT monitoring a given feature.
     * The returned information are the following:
     *     q            the main parameter ( P(X>z_q) < q )
     *     bounded      If bounded mode
     *     max_excess   Maximum number of stored excesses (bounded mode)
     *     alert        If triggering alert
     *     up           If monitoring too high values
     *     down         If monitoring too low values
     *     n_init       number of observation to perform calibration
     *     level        level of the update threshold (0<l<1)
     * @param msg the incoming DBUS message (argument: a string for the name of the feature)
     */
  void reply_spotconfig(DBusMessage *msg);

  /**
     * A function to reply to an introspection call
     * @param msg initial message
     */
  void reply_introspection(DBusMessage *msg);

  /**
     * A function returning an error on DBUS
     * @param msg initial message raising the error
     * @param message the error message (some explanations about the error)
     */
  void error(DBusMessage *msg, string message);

  /**
     * A function returning a basic message on DBUS
     * @param msg initial message
     * @param message the reply
     */
  void reply(DBusMessage *msg, string message);

  /**
     * Reply function for the reset method
     * It returns nothing
     * @param msg the incoming DBUS message
     */
  void reply_reset(DBusMessage *msg);

public:
  /**
     * Constructor
     * @param dsp the pointer to the dispotcher
     */
  Dbuspotcher(Dispotcher *dsp);

  /**
     * Close the DBUS conenction
     */
  ~Dbuspotcher();

  /**
     * Start to listen messages on DBUS
     */
  void listen();
};

#endif //BRO_SPOTD_DBUSPOTCHER_H
