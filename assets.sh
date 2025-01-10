#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py


# rm -rf data
# mkdir -p data
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
    --input ./models/coin.obj \
    --output data/coin.bin \
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
    --input ./models/pole.obj \
    --output data/pole.bin \
    --texture 256 256


python3 $OBJ2DL \
    --input ./models/sloppy.obj \
    --output data/sloppy.bin \
    --texture 256 256


python3 $OBJ2DL \
    --input ./models/number0.obj \
    --output data/number0.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/number1.obj \
    --output data/number1.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/number2.obj \
    --output data/number2.bin \
    --texture 256 256


python3 $OBJ2DL \
    --input ./models/number3.obj \
    --output data/number3.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/number4.obj \
    --output data/number4.bin \
    --texture 256 256


python3 $OBJ2DL \
    --input ./models/number5.obj \
    --output data/number5.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/number6.obj \
    --output data/number6.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/number7.obj \
    --output data/number7.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/number8.obj \
    --output data/number8.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/number9.obj \
    --output data/number9.bin \
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
