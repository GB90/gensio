#
#  gensio - A library for abstracting stream I/O
#  Copyright (C) 2018  Corey Minyard <minyard@acm.org>
#
#  SPDX-License-Identifier: LGPL-2.1-only
#

from utils import *
import gensio

print("Test telnet over sctp accepter connect")
TestAcceptConnect(o, "telnet,sctp,0", "telnet,sctp,0",
                  "telnet,sctp,localhost,", do_small_test)

