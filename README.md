# gr-streamsink
A GNURadio sink block to stream the audio to SHOUTcast or Icecast.

## Thanks
Thanks to Daniel Nöthen, his code from [butt](http://butt.sourceforge.net/) (broadcast using this tool) was very helpful.

## Dependencies
GNURadio and the LAME library (libmp3lame-dev) must be installed and you need a SHOUTcast or Icecast server running.

## Installation
    $ mkdir build
    $ cd build
    $ cmake ../
    $ make
    $ sudo make install
    $ sudo ldconfig

## Usage
The block is available in grc (gnuradio-companion). See the examples
directory for some flow graphs and python applications.

### Usage in python
Import:
````
import streamsink
```

Create the sink block:
```
sink = streamsink.streamsink(
        samplerate,         # The input samplerate
        bitrate,            # The mp3 bitrate
        server_type,        # "shoutcast" or "icecast"
        address,            # IP address or hostname of the server
        port,
        password,
        icecast_user,       # Only required if using Icecast
        icecast_mountpoint) # Only required if using Icecast
```

Add additional info and properties for the SHOUTcast / Icecast stream:
```sink.set_stream_public(False)
sink.set_stream_name("Your stream name")
sink.set_stream_genre("Your genre")
sink.set_stream_url("example.org")
sink.set_stream_description("Your description") # Only for Icecast
```
