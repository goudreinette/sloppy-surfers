#pragma once

#include <NEMain.h>

#include "teapot_bin.h"
#include "sphere_bin.h"
#include "trein_bin.h"

#include "texture.h"


namespace test_dual_screen {
    NE_Material *material;
    
    struct SceneData {
        NE_Camera *cameraTop, *cameraBottom;
        NE_Model *train, *track, *ground;
    };


    // game
    float speed = 0.1;
    int score = 0;
    int coins = 0; // maybe?


    // lanes
    int lane_gap = 2;
    int current_lane = 0;


    // trains
    namespace trains {
        struct train {
            float z = 20;
            float scale = 2;
            float x;
        };
        
        train trains[] = {
            // train{.z = 50 + rand() % 20, .x = -lane_gap}, // left
            train{.z =  50 + rand() % 20, .x = 0}, // center
            // train{.z =  50 + rand() % 20, .x = lane_gap} // right
        };

        void update(int cam_z) {
            for (train &t: trains) {
                t.z += -0.3 - speed / 4;

                // respawn in the distance
                if (t.z < cam_z - 10) {
                    t.z = cam_z + 50 + rand() % 20;
                }
            }     
        }
        
        void draw(SceneData* scene)  {
            // draw train
            for (train &t: trains) {
                NE_ModelSetCoord(scene->train, t.x, -3, t.z);
                NE_ModelDraw(scene->train);
            }
        }
    }



    // camera ---------------------------
    namespace cameras {
        float cam_y = 0.0;
        float cam_y_bottom = 0.0;
        float cam_z = -10.0;
        float cam_x = 0.0;

        float target_cam_y = cam_y;
        float target_cam_y_bottom = cam_y;

        float target_cam_x = cam_x;
        float target_cam_z = cam_z;

        float z_look_at_distance = -10;


        void update(SceneData* scene) {
            // Jump up for trains 
            trains::train train_in_lane = trains::trains[current_lane];
            if (train_in_lane.z && (train_in_lane.z < cam_z + 20)) { // high camera
                target_cam_y = 7;
                target_cam_y_bottom = 2.5;
            } else { // low camera
                target_cam_y = -1;
                target_cam_y_bottom = -1.75;
            }

            // Camera in lane
            if (current_lane == -1) {
                target_cam_x = -lane_gap;
            } else if (current_lane == 0) {
                target_cam_x = 0;
            } else if (current_lane == 1) {
                target_cam_x = lane_gap;
            }

            // Lerp camera
            float lerp_speed = utils::map(speed, 0.0, 3.0, 0.1, 0.6);
            cam_x = utils::lerp(cam_x, target_cam_x, lerp_speed);
            cam_y = utils::lerp(cam_y, target_cam_y, lerp_speed);
            cam_y_bottom = utils::lerp(cam_y_bottom, target_cam_y_bottom, lerp_speed);


            NE_CameraSet(scene->cameraTop,
                cam_x, cam_y, cam_z, // position
                cam_x, -3, cam_z - z_look_at_distance, // look at
                0, 1, 0); // up

            NE_CameraSet(scene->cameraBottom,
                cam_x, cam_y_bottom, cam_z - 4, // position -- FIXME
                cam_x, -20, cam_z - z_look_at_distance - 4, // look at
                0, 1, 0); // up
        }
    }


    // ground ---------------------------
    namespace ground {
        int ground_start_z = 0;

        void update() {
            // Update ground 
            if (ground_start_z < cameras::cam_z - 66) {
                ground_start_z += 66;
            }
        }

        void draw(SceneData* scene) {
            // draw ground
            NE_ModelScale(scene->ground, 5, 5, 10);
            NE_ModelSetCoord(scene->ground, 0, -3.1, ground_start_z + 130);
            NE_ModelDraw(scene->ground);
            NE_ModelSetCoord(scene->ground, 0, -3.1, ground_start_z);
            NE_ModelDraw(scene->ground);
            NE_ModelSetCoord(scene->ground, 0, -3.1, ground_start_z + 66);
            NE_ModelDraw(scene->ground);
        }
    }


    // tracks  ---------------------------
    namespace tracks {
        float track_height = -3;
        const int num_track_parts = 7;
        int track_gap = 7;
        int track_start_z = 0;

        void update() {
            if (track_start_z < cameras::cam_z - 10) {
                track_start_z += 7;
            }
        }

        void draw(SceneData* scene) {
            // draw tracks
            for (int track_i = 0; track_i < num_track_parts; ++track_i) {
                for (int lane = -1; lane <= 1; ++lane) {
                    NE_ModelSetCoord(scene->track, lane * lane_gap - .200, track_height, track_start_z + track_i * track_gap);
                    // NE_ModelRotate(scene->track, 0, 90, 0);
                    NE_ModelSetRot(scene->track, 0, 128, 0);
                    NE_ModelDraw(scene->track);
                }
            }
        }
    }

    
    

    // DRAW the two screens ----------------------------
    void Draw3DSceneTop(void *arg) {
        SceneData *scene = (SceneData*) arg;

        // NE_ClearColorSet(NE_Green, 31, 63);

        NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);

        NE_CameraUse(scene->cameraTop);

        // Draw ground, tracks and trains
        ground::draw(scene);        
        tracks::draw(scene);
        trains::draw(scene);
    }

    void Draw3DSceneBottom(void *arg) {
        SceneData *scene = (SceneData*) arg;

        // NE_ClearColorSet(NE_Red, 31, 63);

        NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);

        NE_CameraUse(scene->cameraBottom);

        // Draw ground, tracks and trains
        ground::draw(scene);
        tracks::draw(scene);
        trains::draw(scene);
    }


    // INIT graphics objects ------------------------------
    void init_all(SceneData *scene) {
        // Allocate objects...
        scene->train = NE_ModelCreate(NE_Static);
        scene->track = NE_ModelCreate(NE_Static);
        scene->ground = NE_ModelCreate(NE_Static);

        scene->cameraTop = NE_CameraCreate();
        scene->cameraBottom = NE_CameraCreate();

        // Load models
        NE_ModelLoadStaticMesh(scene->train, trein_bin);
        NE_ModelLoadStaticMesh(scene->track, track_bin);
        NE_ModelLoadStaticMesh(scene->ground, ground_bin);

        // Create and set shared material
        material = NE_MaterialCreate();
        NE_MaterialTexLoad(material, NE_RGB5, 256, 256, NE_TEXGEN_TEXCOORD, textureBitmap);
        NE_MaterialSetProperties(material,
            RGB15(31, 31, 31), // diffuse
            RGB15(5, 5, 5), // ambient
            RGB15(15, 15, 15), // specular
            RGB15(15, 15, 15), // emission
            false, // vtxcolor
            false // useshininess
        );

        NE_ModelSetMaterial(scene->train, material);
        NE_ModelSetMaterial(scene->track, material);
        NE_ModelSetMaterial(scene->ground, material);

        // Set light color and direction
        NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);
    }




    // MAIN -------------------------------------------
    int main() {
        SceneData scene = { 0 };

        // This is needed for special screen effects
        irqEnable(IRQ_HBLANK);
        irqSet(IRQ_VBLANK, NE_VBLFunc);
        irqSet(IRQ_HBLANK, NE_HBLFunc);

        // Init dual 3D mode and console
        NE_InitDual3D_FB();
        NE_InitConsole();

        init_all(&scene);

        bool console = true;


        while (1) {
            // Move forward
            cameras::cam_z += speed;

            NE_WaitForVBL(NE_CAN_SKIP_VBL);

            // Draw 3D scenes
            NE_ProcessDualArg(Draw3DSceneBottom, Draw3DSceneTop, &scene, &scene);

            // Refresh keys
            scanKeys();
            uint32_t keys = keysHeld();
            uint32_t kdown = keysDown();

            // Speed and lane switching
            if (keys & KEY_UP) {
                speed += 0.01;
            }
            if (keys & KEY_DOWN) {
                speed -= 0.01;
            }
            if (kdown & KEY_LEFT) {
                // NE_ModelRotate(scene.train, 0, 2, 0);
                if (current_lane < 2) current_lane++;
            }
            if (kdown & KEY_RIGHT) {
                // NE_ModelRotate(scene.train, 0, -2, 0);
                if (current_lane >= 0) current_lane--;
            }

            ground::update();
            tracks::update();
            trains::update(cameras::cam_z);
            cameras::update(&scene);
        }

        return 0;
    }

}