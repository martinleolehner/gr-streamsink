#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright 2015 Martin Lehner - HES-SO Valais-Wallis
#
# This file is part of gr-streamsink
#
# gr-streamsink is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# gr-streamsink is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with gr-streamsink. If not, see <http://www.gnu.org/licenses/>.
#

from gnuradio import gr, gr_unittest
from gnuradio import blocks
import streamsink_swig as streamsink


class qa_streamsink (gr_unittest.TestCase):

    def setUp (self):
        self.tb = gr.top_block ()

        def tearDown (self):
            self.tb = None

            def test_001_t (self):
                # set up fg
                src_data = (0.1, 0.2, 0.3, 0.2, 0.1, 0, -0.2, -0.4, -0.6, -1, -0.6, -0.4, -0.2, 0)
                src = blocks.vector_source_f(src_data)
                sink = streamsink.streamsink(48000, 128, "SHOUTcast", "127.0.0.1", 8001, "-!1streamSDR9$_")
                self.tb.connect(src, sink)

                self.tb.run ()
                # check data


                if __name__ == '__main__':
                    gr_unittest.run(qa_streamsink, "qa_streamsink.xml")
