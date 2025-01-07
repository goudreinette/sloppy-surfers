#pragma once

#include <NEMain.h>

#include "teapot_bin.h"
#include "sphere_bin.h"
#include "trein_bin.h"

#include "mp3.h"

namespace test_dual_screen {
    NE_Material *material;
    
    typedef struct {
        NE_Camera *CameraTop, *CameraBottom;
        NE_Model *Teapot, *Sphere;
    } SceneData;


    // camera
    float cam_y = 0.0;
    float cam_z = -10.0;
    float cam_x = 0.0;

    float target_cam_y = cam_y;
    float target_cam_x = cam_x;
    float target_cam_z = cam_z;
    
    // speed
    float speed = 0.2;

    // tracks
    float track_height = -3;
    const int num_track_parts = 7;
    int track_gap = 7;
    int track_start_z = 0;

    // ground
    int ground_start_z = 0;

    // lanes
    int distance = 6;
    int current_lane = 1;

    float z_distance = -10;


    struct train {
        float z = 20;
        float scale = 2;
        float x;
    };

    train trains[] = {
        train{.z = 50 + rand() % 20, .x = -distance}, // left
        train{.z =  50 + rand() % 20, .x = 0}, // center
        train{.z =  50 + rand() % 20, .x = distance} // right
    };


    

    void Draw3DSceneTop(void *arg)
    {
        SceneData *Scene = (SceneData*) arg;

        NE_ClearColorSet(NE_Green, 31, 63);

        NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);


        NE_CameraUse(Scene->CameraTop);

        for (train &t: trains) {
            NE_ModelSetCoord(Scene->Teapot, t.x, -3, t.z);
            NE_ModelDraw(Scene->Teapot);
        }

        // NE_ModelDraw(Scene->Teapot);
    }

    void Draw3DSceneBottom(void *arg)
    {
        SceneData *Scene = (SceneData*) arg;

        NE_ClearColorSet(NE_Red, 31, 63);

        NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);


        NE_CameraUse(Scene->CameraBottom);

        for (train &t: trains) {
            NE_ModelSetCoord(Scene->Teapot, t.x, -3, t.z);
            NE_ModelDraw(Scene->Teapot);
        }

        // NE_ModelDraw(Scene->Teapot);
    }

    void init_all(SceneData *Scene)
    {
        // Allocate objects...
        Scene->Teapot = NE_ModelCreate(NE_Static);
        Scene->Sphere = NE_ModelCreate(NE_Static);
        Scene->CameraTop = NE_CameraCreate();
        Scene->CameraBottom = NE_CameraCreate();

        // Setup camera
        NE_CameraSet(Scene->CameraTop,
                    0, 2.5, z_distance,
                    0, 0, 0,
                    0, 1, 0);
        
        NE_CameraSet(Scene->CameraBottom,
                    0, 2, z_distance,
                    0, -20, 0,
                    0, 1, 0);

        // Load models
        NE_ModelLoadStaticMesh(Scene->Teapot, trein_bin);

        // Material
        material = NE_MaterialCreate();
        NE_MaterialTexLoad(material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, mp3Bitmap);
        NE_MaterialSetProperties(material,RGB15(31, 31, 31),RGB15(5,5,5),RGB15(15, 15, 15),RGB15(15, 15, 15),false, false);

        NE_ModelSetMaterial(Scene->Teapot, material);
        NE_ModelSetMaterial(Scene->Sphere, material);

        // Set light color and direction
        NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);
    }

    int main()
    {
        SceneData Scene = { 0 };

        // This is needed for special screen effects
        irqEnable(IRQ_HBLANK);
        irqSet(IRQ_VBLANK, NE_VBLFunc);
        irqSet(IRQ_HBLANK, NE_HBLFunc);

        // Init dual 3D mode and console
        NE_InitDual3D_FB();
        NE_InitConsole();

        init_all(&Scene);

        bool console = true;

        while (1)
        {
            NE_WaitForVBL(NE_CAN_SKIP_VBL);

            // Draw 3D scenes
            NE_ProcessDualArg(Draw3DSceneBottom, Draw3DSceneTop, &Scene, &Scene);

            // Refresh keys
            scanKeys();
            uint32_t keys = keysHeld();
            uint32_t kdown = keysDown();

            for (train &t: trains) {
                t.z += -0.3 - speed / 4;

                // respawn in the distance
                if (t.z < cam_z - 10) {
                    t.z = cam_z + 50 + rand() % 20;
                }
            }              


            // Rotate model
            if (keys & KEY_UP)
            {
                NE_ModelTranslate(Scene.Sphere, 0, 0, 0.5);
                NE_ModelTranslate(Scene.Teapot, 0, 0, 0.5);
            }
            if (keys & KEY_DOWN)
            {
                NE_ModelTranslate(Scene.Sphere, 0, 0, -.5);
                NE_ModelTranslate(Scene.Teapot, 0, 0, -.5);
            }
            if (keys & KEY_RIGHT)
            {
                NE_ModelRotate(Scene.Sphere, 0, 2, 0);
                NE_ModelRotate(Scene.Teapot, 0, 2, 0);
            }
            if (keys & KEY_LEFT)
            {
                NE_ModelRotate(Scene.Sphere, 0, -2, 0);
                NE_ModelRotate(Scene.Teapot, 0, -2, 0);
            }

            // Camera update
            train train_in_lane = trains[0];
            if (train_in_lane.z < cam_z + 20) {
                target_cam_y = 2.5;
            } else {
                target_cam_y = -2.5;
            }

            if (current_lane == -1) {
                target_cam_x = -distance;
            } else if (current_lane == 0) {
                target_cam_x = 0;
            } else if (current_lane == 1) {
                target_cam_x = distance;
            }

            float lerp_speed = utils::map(speed, 0.0, 3.0, 0.1, 0.6);
            cam_y = utils::lerp(cam_y, target_cam_y, lerp_speed);

            // NE_CameraSet(Scene.CameraTop,
            //      cam_z, cam_y, cam_x,  // Position
            //      cam_z + 10, 0, cam_x,  // Look at
            //      0, 1, 0); // Up direction

            NE_CameraSet(Scene.CameraTop,
                0, cam_y + .5, z_distance,
                0, 0, 0,
                0, 1, 0);


            NE_CameraSet(Scene.CameraBottom,
                0, cam_y, z_distance,
                0, -20, 0,
                0, 1, 0);
        }

        return 0;
    }

}