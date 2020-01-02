#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

Import("env")

EXTLIBS_DIR = env.subst("$PROJECT_DIR/extlib")
CRYPTOLIB_DIR = EXTLIBS_DIR + os.sep + "Crypto"

if not os.path.exists(CRYPTOLIB_DIR):
    print("Cloning Arduino Crypto lib ...")
    env.Execute(
        "git clone --depth 32 -b valid_json https://github.com/rpoisel/arduinolibs.git " + CRYPTOLIB_DIR)
else:
    print("Checking for updates of Crypto lib ...")
    env.Execute("git --work-tree=" + CRYPTOLIB_DIR + " --git-dir=" +
                CRYPTOLIB_DIR + os.sep + ".git pull origin valid_json --depth 32")
