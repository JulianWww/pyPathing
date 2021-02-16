"setup script"

import json
from os.path import dirname, join
from os import system
from sys import executable
import warnings

class unbuiltCython(Warning):
    "the cython extensions did not build so they will be rebuilt"

def run():
    "build cython files"

    warnings.warn(unbuiltCython("rebuilding all cython modules for the current python version\n\tthis will take some time"))
    #update logg utils json file

    path = dirname(__file__)
    #print( f"{executable} {join(path, u"utils/Optimizations/build.py")}")
    buildScript_path = join(path, "scr/backup_build.py")
    outputScriptPath = join(path, "scr")
    import os
    for file in [f for f in os.listdir(dirname(executable)) if f.endswith('.exe')]:
        if file != "Removeepydoc.exe":
            try:
                    system(f"{join(dirname(executable), file)} {buildScript_path} build_ext --build-lib {outputScriptPath}")
            except:
                pass

if __name__ == "__main__":
    run()