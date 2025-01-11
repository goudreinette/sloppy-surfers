#include <nds.h>
#include <stdio.h>
#include <NEMain.h>
#include <NEGeneral.h>
#include <math.h>
#include <filesystem.h>
#include <nf_lib.h>

// Models and graphics
#include "heart_bin.h"
#include "texture2.h"
#include "heart.h"
#include "rainbow.h"
#include "texture3.h"
#include "texture.h"
#include "paper.h"
#include "sloppy_bin.h"

// Includes
#include "sloppy_surfers.hpp"




int main(int argc, char *argv[]) {
    sloppy_surfers::main();

//     nitroFSInit(NULL);
//     NF_SetRootFolder("NITROFS");

//     mic::setup();

//     // setup scene
//     scene_data scene = {
//         .camera = NE_CameraCreate()
//     };


//     irqEnable(IRQ_HBLANK);
//     irqSet(IRQ_VBLANK, NE_VBLFunc);
//     irqSet(IRQ_HBLANK, NE_HBLFunc);

//     // Init Nitro Engine in normal 3D mode
//     NE_Init3D();

//     // libnds uses VRAM_C for the text console, reserve A and B only
//     NE_TextureSystemReset(0, 0, NE_VRAM_AB);

//     // Init console in non-3D screen
//     // Setup models and scenes
//     load_mp3_material();

//     hearts_and_comments::load_assets(&scene);
//     wout_bust::load_assets(&scene);
//     trein::load_assets(&scene);


//     // Set coordinates for the camera
//     NE_CameraSet(scene.camera,
//                  -10, 0, 0,  // Position
//                   0, 0, 0,  // Look at
//                   0, 1, 0); // Up direction

  

//     // We set up a light and its color
//     NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);
// //    setBackdropColorSub(RGB8(95,205,228)); // light


//     consoleDemoInit();
//     // Wrap values of parameters

//     // Enable/update fog
// //    NE_FogEnable(2, RGB8(95,205,228), 31, 4, 0x7C00);
//     NE_FogEnable(2, RGB8(0,0,0), 31, 5, 0x7C00);

// //    NE_SpecialEffectNoiseConfig(31);
//     // NE_SpecialEffectSet(NE_NOISE);



//     // Load sloppy text
//     scene.sloppy_text = NE_ModelCreate(NE_Static);

//     text_material = NE_MaterialCreate();
//     // Load mesh from RAM and assign it to the object "Model".
//     NE_ModelLoadStaticMesh(scene.sloppy_text, sloppy_bin);
//     // Assign texture to model...
//     NE_ModelSetMaterial(scene.sloppy_text, text_material);
//     // Set some properties to the material
//     NE_MaterialSetProperties(text_material,
//                              RGB15(31, 31, 31), // Diffuse
//                              RGB15(15, 15, 15), // Ambient
//                              RGB15(31, 31, 31),    // Specular
//                              RGB15(0, 0, 0),    // Emission
//                              false, true);     // Vertex color, use shininess table


    // while (true)
    // {
    //     t++;

    //     // Wait for next frame
    //     NE_WaitForVBL(NE_UpdateFlags::NE_UPDATE_PHYSICS);
            
    //     // Draw scene
    //     NE_ProcessArg(draw_3d_scene, &scene);
    // }

    return 0;
}


