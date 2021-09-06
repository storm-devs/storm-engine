import glob
import os.path

import gamesave_convert

if __name__ == '__main__':
    files = glob.iglob('./SAVE/**/*', recursive=True)
    for filename in files:
        if os.path.isfile(filename):
            gamesave_convert.process_file(filename)
