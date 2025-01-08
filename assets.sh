#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py


rm -rf data
mkdir -p data
#
#python3 $OBJ2DL \
#    --input ./models/clippy.obj \
#    --output data/clippy.bin \
#    --texture 256 256
#
#python3 $OBJ2DL \
#    --input ./models/cloud.obj \
#    --output data/cloud.bin \
#    --texture 256 256
#
#
#python3 $OBJ2DL \
#    --input ./models/paper.obj \
#    --output data/paper.bin \
#    --texture 256 256

python3 $OBJ2DL \
    --input ./models/heart.obj \
    --output data/heart.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/wout_head_low.obj \
    --output data/wout_head_low.bin \
    --texture 256 256

  python3 $OBJ2DL \
      --input ./models/wout_cape.obj \
      --output data/wout_cape.bin \
      --texture 256 256

python3 $OBJ2DL \
    --input ./models/mp3.obj \
    --output data/mp3.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/swf.obj \
    --output data/swf.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/trein.obj \
    --output data/trein.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/track.obj \
    --output data/track.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/ground.obj \
    --output data/ground.bin \
    --texture 256 256


python3 $OBJ2DL \
    --input ./models/sloppy.obj \
    --output data/sloppy.bin \
    --texture 256 256

# for background in assets/affine-bgs/*.png; do
#     grit $background -ftb -fh! -gb -gB16
# done

# for background in assets/backgrounds/*.png; do
#     grit $background -ftb -fh! -gT000000 -gt -gB8 -mR8 -mLs
# done

# #for sprite in assets/sprites/*.png; do
# #    grit $sprite -ftb -fh! -gT000000 -gt -gB8 -m!
# #done

# for file in *.bin; do
#     mv -- "$file" "${file%.bin}"
# done

# mv *.pal *.map *.img ./nitrofiles/
