# bro-spotd

## Introduction

`bro-spotd` is a daemon client for [Bro IDS](https://www.bro.org/). It monitors network features produced by Bro in a smart way.
Actually, it uses [`libspot`](https://asiffer.github.io/libspot/) so as to perform intelligent thresholding on streaming data. Therefore, it triggers events when abnormal behaviours occur.

In details, `bro-spotd` uses [Broccoli](https://www.bro.org/sphinx/components/broccoli/broccoli-manual.html)
to interact with a Bro controller (receiving measures, sending abnormal events).
The default possible measures are defined in the Bro script **stats.bro**
but it can been extended with other Bro scripts.

With a basic .ini file, you can decide which feature you want to monitor and also
the corresponding `SPOT` parameters (actually a default profile exists).

For each selected feature, `bro-spotd` launches a `SPOT` instance to monitor it. 
These instances are able to send events to the Bro controller (when a feature takes an abnormal value). These events are logged by Bro (generally in /var/log/bro).

You can interact with the daemon through [D-Bus](https://en.wikipedia.org/wiki/D-Bus).


## Installation

Naturally, you have to install `bro` and `broctl` (the controller). The sources can be found on https://www.bro.org/download/index.html. However, if you have a debian system:
```bash
sudo apt install bro bro-aux bro-common broctl
```

### Debian 

The easiest way to install `bro-spotd` is to install the debian package from my personal package archive:

```bash
sudo add-apt-repository ppa:asiffer/libspot
sudo apt update
sudo apt install bro-spotd
```


### Manual 

First you have to install `libspot` (see https://asiffer.github.io/libspot/download/).
Then, you just have to clone the git repository, build and install.
```terminal
git clone https://github.com/asiffer/bro-spotd.git
cd bro-spotd/
make
sudo make install
```


## Getting started

Initially, you have to run the Bro controller. First, you can do it with the root user:
```terminal
sudo broctl
```
However, it is possible to avoid it by setting the right network capabilites to the `bro` binary (see https://www.bro.org/documentation/faq.html#with-linux-capabilities).
Once started, from the CLI, you can start your Bro instances with the `deploy` command. The launched instances are referenced on the file `/etc/bro/node.cfg` which may look like this (for a single instance):
```ini
# This is a complete standalone configuration. Most likely you will
# only need to change the interface.
[bro]
type=standalone
host=localhost
interface=eno1
```

Moreover, it makes `bro` listen on an interface (`localhost:47760` by default). Particularly, `bro-spotd` uses it to communicate with the engine.
The complete details about the Bro controller can be found [here](https://www.bro.org/sphinx/components/broctl/README.html)


### Configuration

`bro-spotd` can be parametrized through the config file `/etc/bro-spotd/bro-spotd.ini`. It gathers several things but the most important is the beginning. 
```ini
# ----------------------------------------------------------------------------#
# ---------------------------- bro-spotd Config ------------------------------#
# ----------------------------------------------------------------------------#
[default]
bro_type = 5
q = 1e-4
n_init = 2000
level = 0.99
up = true
down = false
bounded = true
max_excess = 200

#[active_tcp_conns]
#bro_type = 3

#[pkts_link]
#bro_type = 3

#[pkts_dropped]
#bro_type = 3
```
The idea is to parametrize a `SPOT` instance for all the network features you want to monitor. A default profile is given to ease this step. The description of the `SPOT` parameters can be found [here](https://asiffer.github.io/libspot/tuto/).
The available network features are thos produces by the bro script `stats.bro`. They are all referenced at the end of the configuration file. Thus you can add the feature you want by adding the corresponding section (see the examples above).
A paramount thing is to precise the `bro_type`, this is just the type used by `Bro` to store and transfer the data. These types are also precised for each available monitored features (the .ini file includes also the conversion table between the bro type and the corresponding).

Naturally, it analyzes the features already extracted by `Bro`. But, `Bro` provide a powerful language which allow you to extract custom information. 


### Running the service

`bro-spotd` comes with a userland `systemd` service. So, the best way to run the program is to start the service:
```terminal
systemctl --user start dbus-bro.spot
```

### D-BUS control

Once `bro-spotd` is started, you can retrieve some information through `D-Bus`. Many clients exist so as to send message on the bus. Here we give examples with the basic linux command:

```terminal
dbus-send --session --print-reply --dest=bro.spot / INTERFACE.METHOD [ARGUMENT]
```

Two interfaces are available `bro.spot.Common` and `bro.spot.Spot`. The first is for the general purposes (not specific to a monitored feature) although the second is relative to a `SPOT` instance. The available methods are the following:

| Interface             | Method ARGUMENT     | Description                                |
|-----------------------|---------------------|--------------------------------------------|
| `bro.spot.Common`     | `Features`          | Get all the monitored features             |
|                       | `Kill`              | Kill the program                           |
| `bro.spot.Spot`       | `Status STRING`     | Get the status of a Spot instance          |
|                       | `Config STRING`     | Get the config of a Spot instance          |
|                       | `Reset STRING`      | Reset a Spot instance                      |

On `bro.spot.Spot` interface, methods require an argument (`STRING`) which is the name of the feature.


### Detected anomalies

When a `SPOT` instance detect an abnormal value, it triggers an event which will be logged by `Bro`. The log files can generally be found in `/var/log/bro`. The anomalies found are registered through `spot_anomaly`. Later, I hope they will be handled by the `Notice` framework (the `Bro` framework to trigger noticeable events).

## Notes

This program is far from being completed. You may have errors or merely nothing. Currently, this is just a proof of concept to show that we can use a bit of AI to do *behavorial detection*.