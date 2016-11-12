# SDLaucat

Stream audio recorded from SDL to your [sndio] server

[![Vinyl Cat](https://img.youtube.com/vi/K7dcSr04G8s/0.jpg)](https://www.youtube.com/watch?v=K7dcSr04G8s)

Combined with [VB-Cable] (or similar software) on Windows, you can stream
system audio on Windows to your [sndio] server over the
network running on Linux, [FreeBSD], or [OpenBSD].  Latency is good
enough to play games and/or watch movies.

[VB-Cable]: http://vb-audio.pagesperso-orange.fr/Cable/
[sndio]: http://www.sndio.org/
[FreeBSD]: https://www.freebsd.org/
[OpenBSD]: https://www.openbsd.org/

# Usage

First set the default playback device to `CABLE Input` and the capture device
to `CABLE Output`.  Next set the max latency to 1536 samples in the [VB-Cable]
Control Panel.  This is the lowest latency setting I can choose before things
either do not work anymore or stuttering starts.  YMMV.  SDLaucat's default
parameters are set to work with this setting.


```
usage: SDLaucat [-b size] [-c chan] [-d] [-e enc] [-f sndiodev] [-g sdldev]
        [-l] [-r rate] [-s samples]
```

It's a console application.  Running it like `SDLaucat.exe -f snd@sndioserver/0`
should be enough in most cases.

On Windows it'll use the default capture device by default, but can be set to
another device with the `-g` flag.

`SDLaucat.exe -l` will get you a list of capture devices that SDL knows and
their indices.

There are some parameters to control latency, buffer sizes, rate, encoding,
etc.  These mostly follow the parameters from [sndiod(8)].

By default SDLaucat will daemonize itself.  If you intend to experiment with
the settings use `-d` to not go into the background.

If you adjust the rate and encoding you might want to adjust the quality
setting on the recording device `CABLE Output` too.

[sndiod(8)]: http://man.openbsd.org/sndiod.8

# How do I quit SDLaucat?

The Task Manager for now ;-)

# Wanted

It should be very easy to extend this to use recording devices connected to
your [sndio] server and stream them into `CABLE Output`, so that applications
like e.g. Skype can use them via `CABLE Input`.

# License

Copyright (c) 2016 Tobias Kortkamp

Distributed under the terms of the ISC license
