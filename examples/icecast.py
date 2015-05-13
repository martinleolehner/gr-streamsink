#!/usr/bin/env python2
##################################################
# GNU Radio Python Flow Graph
# Title: Icecast
# Generated: Wed May 13 07:59:42 2015
##################################################

from gnuradio import audio
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser

import streamsink

class icecast(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Icecast")

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 48000

        ##################################################
        # Blocks
        ##################################################
        self.audio_source = audio.source(samp_rate, "", True)

        self.streamsink = streamsink.streamsink(samp_rate, 128, "icecast", "127.0.0.1", 8000, "your_password", "source", "/stream")
        self.streamsink.set_stream_name("Your stream name")
        self.streamsink.set_stream_url("www.example.org")
        self.streamsink.set_stream_genre("Your genre")
        self.streamsink.set_stream_description("Your description")

        ##################################################
        # Connections
        ##################################################
        self.connect((self.audio_source, 0), (self.streamsink, 0))


if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    tb = icecast()
    tb.start()
    try:
        raw_input('Press Enter to quit: ')
    except EOFError:
        pass
    tb.stop()
    tb.wait()
