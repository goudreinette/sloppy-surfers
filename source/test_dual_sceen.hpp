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
        NE_Model *train, *track, *ground;
    } SceneData;


    // camera
    float cam_y = 0.0;
    float cam_y_bottom = 0.0;
    float cam_z = -10.0;
    float cam_x = 0.0;

    float target_cam_y = cam_y;
    float target_cam_y_bottom = cam_y;

    float target_cam_x = cam_x;
    float target_cam_z = cam_z;
    
    // speed
    float speed = 0.1;

    // tracks
    float track_height = -3;
    const int num_track_parts = 7;
    int track_gap = 7;
    int track_start_z = 0;

    // ground
    int ground_start_z = 0;

    // lanes
    int distance = 6;
    int current_lane = 0;

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


    
    void draw_tracks(SceneData* Scene) 
    {
        // draw tracks
        for (int track_i = 0; track_i < num_track_parts; ++track_i) {
            for (int lane = -1; lane <= 1; ++lane) {
                NE_ModelSetCoord(Scene->track, lane * distance - .200, track_height, track_start_z + track_i * track_gap);
                // NE_ModelRotate(Scene->track, 0, 90, 0);
                NE_ModelSetRot(Scene->track, 0, 128, 0);
                NE_ModelDraw(Scene->track);
            }
        }
    }

    void draw_trains(SceneData* Scene) 
    {
    // draw train
        for (train &t: trains) {
            NE_ModelSetCoord(Scene->train, t.x, -3, t.z);
            NE_ModelDraw(Scene->train);
        }
    }
    

    void Draw3DSceneTop(void *arg)
    {
        SceneData *Scene = (SceneData*) arg;

        // NE_ClearColorSet(NE_Green, 31, 63);

        NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);


        NE_CameraUse(Scene->CameraTop);
        
        // draw tracks and train
        draw_tracks(Scene);
        draw_trains(Scene);
    }

    void Draw3DSceneBottom(void *arg)
    {
        SceneData *Scene = (SceneData*) arg;

        // NE_ClearColorSet(NE_Red, 31, 63);

        NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);

        NE_CameraUse(Scene->CameraBottom);

        // draw tracks and train
        draw_tracks(Scene);
        draw_trains(Scene);
}

    void init_all(SceneData *Scene)
    {
        // Allocate objects...
        Scene->train = NE_ModelCreate(NE_Static);
        Scene->track = NE_ModelCreate(NE_Static);
        Scene->ground = NE_ModelCreate(NE_Static);

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
        NE_ModelLoadStaticMesh(Scene->train, trein_bin);
        NE_ModelLoadStaticMesh(Scene->track, track_bin);

        // Material
        material = NE_MaterialCreate();
        NE_MaterialTexLoad(material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, mp3Bitmap);
        NE_MaterialSetProperties(material,RGB15(31, 31, 31),RGB15(5,5,5),RGB15(15, 15, 15),RGB15(15, 15, 15),false, false);

        NE_ModelSetMaterial(Scene->train, material);
        NE_ModelSetMaterial(Scene->track, material);

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
            cam_z += speed;

            NE_WaitForVBL(NE_CAN_SKIP_VBL);

            // Draw 3D scenes
            NE_ProcessDualArg(Draw3DSceneBottom, Draw3DSceneTop, &Scene, &Scene);

            // Refresh keys
            scanKeys();
            uint32_t keys = keysHeld();
            uint32_t kdown = keysDown();


            // Update tracks
            int cam_z_int = (int) cam_z;
            if (track_start_z < cam_z - 10) {
                track_start_z += 7;
            }

            // Update trains
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
                NE_ModelTranslate(Scene.train, 0, 0, 0.5);
            }
            if (keys & KEY_DOWN)
            {
                // NE_ModelTranslate(Scene.train, 0, 0, -.5);
            }
            if (kdown & KEY_LEFT)
            {
                // NE_ModelRotate(Scene.train, 0, 2, 0);
                if (current_lane < 2) current_lane++;
            }
            if (kdown & KEY_RIGHT)
            {
                // NE_ModelRotate(Scene.train, 0, -2, 0);
                if (current_lane >= 0) current_lane--;
            }

            // Camera update 
            train train_in_lane = trains[current_lane];
            if (train_in_lane.z < cam_z + 20) {
                target_cam_y = 7;
                target_cam_y_bottom = 2.5;
            } else {
                target_cam_y = -2;
                target_cam_y_bottom = -1.9;
            }

            // Camera in lane
            if (current_lane == -1) {
                target_cam_x = -distance;
            } else if (current_lane == 0) {
                target_cam_x = 0;
            } else if (current_lane == 1) {
                target_cam_x = distance;
            }

            // Lerp camera
            float lerp_speed = utils::map(speed, 0.0, 3.0, 0.1, 0.6);
            cam_x = utils::lerp(cam_x, target_cam_x, lerp_speed);
            cam_y = utils::lerp(cam_y, target_cam_y, lerp_speed);
            cam_y_bottom = utils::lerp(cam_y_bottom, target_cam_y_bottom, lerp_speed);


            NE_CameraSet(Scene.CameraTop,
                cam_x, cam_y, cam_z,
                cam_x, 0, cam_z - z_distance,
                0, 1, 0);

            NE_CameraSet(Scene.CameraBottom,
                cam_x, cam_y_bottom, cam_z,
                cam_x, -20, cam_z - z_distance,
                0, 1, 0);
        }

        return 0;
    }

}