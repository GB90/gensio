#
#  gensio - A library for abstracting stream I/O
#  Copyright (C) 2018  Corey Minyard <minyard@acm.org>
#
#  SPDX-License-Identifier: LGPL-2.1-only
#

from utils import *
import gensio

print("Test accept sctp")
# FIXME - the raddr and laddr areq not tested here, it's hard to
# know what it would be because of sctp multihoming.
a = TestAccept(o, "sctp,localhost,", "sctp,0", do_test, do_close = False)
c = a.io1.control(0, True, gensio.GENSIO_CONTROL_STREAMS, None)
if c != "instreams=1,ostreams=1":
    raise Exception("Invalid stream settings: %s" % c)
a.close()
