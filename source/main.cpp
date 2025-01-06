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
#include "paper.h"
#include "sloppy_bin.h"

// Includes
#include "scene_data.hpp"
#include "storm.hpp"
#include "wout3d.hpp"
#include "trein.hpp"





void draw_3d_scene(void *arg) {
    scene_data *scene = (scene_data*) arg;

    NE_CameraUse(scene->camera);

    scanKeys();

    uint32_t keys_held = keysHeld();
    uint32_t keys_down = keysDown();

    printf("\x1b[2;1Hy4k4r1");
    printf("\x1b[5;1H<3<3<3<3<3<3");

    // controls
    if (keys_down & KEY_L) {
        trein_active = !trein_active;
        if (!trein_active) {
            trein::exit();
        }
    }
    if (keys_down & KEY_R) {
        storm_active = !storm_active;
    }
    if (keys_held & KEY_UP) {
        speed += 0.001;
    }
    if (keys_held & KEY_DOWN) {
        speed -= 0.01;
    }

    printf("\x1b[7;1Hq[spd] %.2f", speed);

    // different scenes and features
//    hearts_and_comments::update_draw_hearts(scene, keys_held);
//    wout_bust::update_draw(scene, keys_held, keys_down);


    // Display sloppy text
    // NE_ModelSetRot(scene->wout_head, 0, ry, 0);
    // NE_ModelScale(scene->sloppy_text, 100, 100, 10);
    // NE_ModelSetCoord(scene->sloppy_text, 0,-12,0);
    // NE_ModelDraw(scene->sloppy_text);


    if (trein_active) {
        trein::update_draw(scene, keys_held, keys_down);
    } else {
        float lerp_speed = 0.2;
        cam_y = lerp(cam_y, target_cam_y, lerp_speed);
        cam_x = lerp(cam_x, target_cam_x, lerp_speed);
        cam_z = lerp(cam_z, target_cam_z, lerp_speed);
    }

    NE_CameraSet(scene->camera,
                 cam_z, cam_y, cam_x,  // Position
                 cam_z + 10, 0, cam_x,  // Look at
                 0, 1, 0); // Up direction
}


void load_mp3_material() {
    material_mp3 = NE_MaterialCreate();
    NE_MaterialTexLoad(material_mp3, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, mp3Bitmap);
    NE_MaterialSetProperties(material_mp3,RGB15(31, 31, 31),RGB15(5,5,5),RGB15(15, 15, 15),RGB15(15, 15, 15),false, false);
}



int main(int argc, char *argv[]) {
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    mic::setup();

    // setup scene
    scene_data scene = {
        .camera = NE_CameraCreate()
    };


    irqEnable(IRQ_HBLANK);
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    // Init Nitro Engine in normal 3D mode
    NE_Init3D();

    // libnds uses VRAM_C for the text console, reserve A and B only
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);

    // Init console in non-3D screen
    // Setup models and scenes
    load_mp3_material();

    hearts_and_comments::load_assets(&scene);
    wout_bust::load_assets(&scene);
    trein::load_assets(&scene);


    // Set coordinates for the camera
    NE_CameraSet(scene.camera,
                 -10, 0, 0,  // Position
                  0, 0, 0,  // Look at
                  0, 1, 0); // Up direction

  

    // We set up a light and its color
    NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);
//    setBackdropColorSub(RGB8(95,205,228)); // light


    consoleDemoInit();
    // Wrap values of parameters

    // Enable/update fog
//    NE_FogEnable(2, RGB8(95,205,228), 31, 4, 0x7C00);
    NE_FogEnable(2, RGB8(0,0,0), 31, 5, 0x7C00);

//    NE_SpecialEffectNoiseConfig(31);
    // NE_SpecialEffectSet(NE_NOISE);



    // Load sloppy text
    scene.sloppy_text = NE_ModelCreate(NE_Static);

    text_material = NE_MaterialCreate();
    // Load mesh from RAM and assign it to the object "Model".
    NE_ModelLoadStaticMesh(scene.sloppy_text, sloppy_bin);
    // Assign texture to model...
    NE_ModelSetMaterial(scene.sloppy_text, text_material);
    // Set some properties to the material
    NE_MaterialSetProperties(text_material,
                             RGB15(31, 31, 31), // Diffuse
                             RGB15(15, 15, 15), // Ambient
                             RGB15(31, 31, 31),    // Specular
                             RGB15(0, 0, 0),    // Emission
                             false, true);     // Vertex color, use shininess table


    while (true)
    {
        t++;

        // Wait for next frame
        NE_WaitForVBL(NE_UpdateFlags::NE_UPDATE_PHYSICS);
            
        // Draw scene
        NE_ProcessArg(draw_3d_scene, &scene);
    }

    return 0;
}


