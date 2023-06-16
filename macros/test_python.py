import os
from ROOT import gSystem
gSystem.Load(f'{os.environ["LILAK_PATH"]}/build/libLILAK.dylib')

from ROOT import TTEventHeader
channel = TTEventHeader()
channel.Print()
