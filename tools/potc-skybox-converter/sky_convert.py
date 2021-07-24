import argparse

from os import path
from ctypes import *

class tx_format(c_int32):
    (TXF_A8R8G8B8,
     TXF_X8R8G8B8,
     TXF_R5G6B5,
     TXF_A1R5G5B5,
     TXF_A4R4G4B4,
     TXF_P8,
     TXF_L8,
     TXF_V8U8,
     TXF_L6V5U5,
     # TXF_DXT1,
     # TXF_DXT2,
     # TXF_DXT3,
     # TXF_DXT4,
     # TXF_DXT5
     ) = (
     21,
     22,
     23,
     25,
     26,
     41,
     50,
     60,
     61,
     # '1TXD',
     # '2TXD',
     # '3TXD',
     # '4TXD',
     # '5TXD'
     )

class tx_head(Structure):
    _fields_ = [
        ('flags', c_int32),
        ('width', c_int32),
        ('height', c_int32),
        ('nmips', c_int32),
        ('format', tx_format),
        ('mip_size', c_int32),
    ]


def convert(filename):
    f = open(filename, "rb")

    header = tx_head()
    f.readinto(header)

    print("Converting '%s'" % filename)
    print("Image format: %s" % header.format.value)
    print("Image width: %d" % header.width)
    print("Image height: %d" % header.height)

    mip_size = header.mip_size

    header.mip_size = mip_size >> 2
    header.height = header.height >> 2
    for output_name in ["sky_fr.tga", "sky_lf.tga", "sky_bk.tga", "sky_rt.tga"]:
        output_file_name = path.join(path.dirname(filename), output_name) + ".tx"
        output_file = open(output_file_name, "wb")
        print("Saving '%s'" % output_file_name)
        output_file.write(header)
        output_file.write(f.read(mip_size >> 2))
        output_file.close()

    f.close()

if (__name__ == "__main__"):
    # define args
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "path", help="Where is the sky_side.tga.tx file located?", type=str)
    args = parser.parse_args()
    convert(str(args.path))
