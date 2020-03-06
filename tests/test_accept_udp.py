#
#  gensio - A library for abstracting stream I/O
#  Copyright (C) 2018  Corey Minyard <minyard@acm.org>
#
#  SPDX-License-Identifier: LGPL-2.1-only
#

from utils import *
import gensio

print("Test accept udp")
TestAccept(o, "udp,localhost,", "udp,localhost,0",
           do_test, io1_dummy_write = "A",
           expected_raddr = "ipv4,127.0.0.1,",
           expected_acc_laddr = "ipv4,127.0.0.1,")
