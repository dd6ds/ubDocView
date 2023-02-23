#!/usr/bin/python3

import os
import sys
from pathlib import Path

organization = "docviewer.ubports"
application = "docviewer.ubports"
old_application = "com.ubuntu.docviewer"

xdg_config_home = Path(os.environ.get("XDG_CONFIG_HOME",
                                      Path.home() / ".config"))
old_config_file = xdg_config_home / organization / f"{old_application}.conf"
new_config_file = xdg_config_home / organization / f"{application}.conf"
if old_config_file.is_file() and not new_config_file.exists():
    old_config_file.rename(new_config_file)

if len(sys.argv) > 1:
    os.execvp(sys.argv[1], sys.argv[1:])
