# bro-spotd

### Introduction

`bro-spotd` is a daemon client for [Bro IDS](https://www.bro.org/). It monitors network features produced by Bro in a smart way.
Actually, it uses [SPOT](https://asiffer.github.io/libspot/) so as to perform intelligent thresholding on streaming data.


### Architecture
In details, `bro-spotd` uses [Broccoli](https://www.bro.org/sphinx/components/broccoli/broccoli-manual.html) 
to interact with a Bro controller (receiving measures, sending abnormal events).
The possible measures are defined in a Bro script which must be loaded
by the Bro controller. 

With a basic .ini file, you can decide which feature you want to monitor and also
the corresponding `SPOT` parameters (actually a default profile exists).

For each selected feature, `bro-spotd` launches a `SPOT` instance to monitor it. 
These instances are able to send events to the Bro controller (when a feature takes an abnormal value).

The daemon itself can be controlled through [D-Bus](https://en.wikipedia.org/wiki/D-Bus)

```commandline
dbus-send --session --print-reply --dest=bro.spot / bro.spot.Common.Features
```
