import json
import subprocess
import os
import hashlib

Import("env")

env = DefaultEnvironment()


# Make sure 'vscode init' is not the current command
def is_pio_build():
    from SCons.Script import DefaultEnvironment
    env = DefaultEnvironment()
    if "IsCleanTarget" in dir(env) and env.IsCleanTarget():
        return False
    return not env.IsIntegrationDump()


import shutil
from pathlib import Path

if is_pio_build() == True:
    print(f"BAMBUDECK PREBUILD")
    # Auto-copy LVGL and TFT_eSPI configs
    libdeps_dir = Path(".pio/libdeps/esp32dev")
    if libdeps_dir.exists():
        lv_dest = libdeps_dir / "lv_conf.h"
        shutil.copy("resources/lv_conf.h", lv_dest)
        tft_dest = libdeps_dir / "TFT_eSPI" / "User_Setup.h"
        if tft_dest.parent.exists():
            shutil.copy("resources/User_Setup.h", tft_dest)
            print("Copied lv_conf.h and User_Setup.h to libdeps")

    result = subprocess.run(['node', 'scripts/download-errors.js'],
                            text=True, check=True, capture_output=True)
    print(result.stdout)
