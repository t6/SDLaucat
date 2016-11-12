# SDLcat

Stream audio recorded from SDL to your [sndio] server

[![Vinyl Cat](https://img.youtube.com/vi/K7dcSr04G8s/0.jpg)](https://www.youtube.com/watch?v=K7dcSr04G8s)

Combined with [VB-Cable] (or similar software) on Windows, you can
stream whatever audio Windows outputs to your [sndio] server over the
network running on Linux, [FreeBSD], or [OpenBSD].  Latency is good
enough to play games and/or watch movies.

[VB-Cable]: http://vb-audio.pagesperso-orange.fr/Cable/
[sndio]: http://www.sndio.org/
[FreeBSD]: https://www.freebsd.org/
[OpenBSD]: https://www.openbsd.org/

# Usage

```
usage: SDLcat [-b size] [-c chan] [-d] [-e enc] [-f sndiodev] [-g sdldev]
        [-l] [-r rate] [-s samples]
```

It's a console application.  Running it like `SDLcat.exe -f
snd@sndioserver/0` should be enough in most cases.

On Windows it'll use the default capture device by default, but can
be set to another device with the `-g` flag.

`SDLcat.exe -l` will get you a list of capture devices that SDL knows
and their indices.

There are some parameters to control latency, buffer sizes, rate,
encoding, etc.  These mostly follow the parameters from [sndiod(8)].

[sndiod(8)]: http://man.openbsd.org/sndiod.8

# License

Copyright (c) 2016 Tobias Kortkamp

Distributed under the terms of the ISC license
