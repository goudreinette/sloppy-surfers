#pragma once

#include <NEMain.h>


#include "trein_bin.h"
#include "coin_bin.h"

#include "texture.h"


#define SKY_COLOR RGB15(18, 28, 31)


namespace test_dual_screen {
    NE_Material *material;
    
    struct SceneData {
        NE_Camera *camera_top, *camera_bottom;
        NE_Model *train, *track, *ground, *coin;
    };


    // game -----------------------------------------------
    enum class GameState {
        Menu,
        Playing, 
        GameOver,
        Credits
    };

    float speed = 0.1;
    int score = 0;
    int coins_collected = 0; // maybe?
    int high_score = 0;
    GameState game_state = GameState::Menu;

    // lanes
    int lane_gap = 2;
    int current_lane = 0;


    // swipe detection ------------------------------------
    namespace swipe_detection {
        int start_touch_x = 0;
        bool is_swiping = false;

        // void update(bool touch_down, int touch_x) { }
        // void swiped_left() { }
        // void swiped_right() { }
    }


    // trains
    namespace trains {
        struct train {
            float z = 20;
            float scale = 2;
            float x;
        };
        
        train trains[] = {
            train{.z = 50 + rand() % 20, .x = -lane_gap}, // left
            train{.z =  50 + rand() % 20, .x = 0}, // center
            train{.z =  50 + rand() % 20, .x = lane_gap} // right
        };

        void update(int cam_z) {
            for (train &t: trains) {
                t.z += -0.3 - speed / 4;

                // respawn in the distance
                if (t.z < cam_z - 10) {
                    t.z = cam_z + 50 + rand() % 100;
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
        float cam_x = 0.0;
        float cam_y = 0.0;
        float cam_y_bottom = 0.0;
        float cam_z = -10.0;
        float cam_z_look_at_bottom_offset = -6;

        float target_cam_x = cam_x;
        float target_cam_y = cam_y;
        float target_cam_y_bottom = cam_y;

        float target_cam_z = cam_z;
        float target_cam_z_look_at_bottom_offset = cam_z_look_at_bottom_offset;
        

        float z_look_at_distance = -10;


        void update(SceneData* scene) {
            // Jump up for trains 
            trains::train train_in_lane = trains::trains[current_lane + 1];
            if (train_in_lane.z && (train_in_lane.z < cam_z + 20)) { // high camera
                target_cam_y = 7;
                target_cam_y_bottom = 2.5;
                target_cam_z_look_at_bottom_offset = -6;
            } else { // low camera
                target_cam_y = 1; //-1;
                target_cam_y_bottom = -1; //-1.75;
                target_cam_z_look_at_bottom_offset = 0;
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
            float lerp_speed = 0.05; //utils::map(speed, 0.0, 3.0, 0.1, 0.3); // camera moves faster when speed increases
            cam_x = utils::lerp(cam_x, target_cam_x, lerp_speed);
            cam_y = utils::lerp(cam_y, target_cam_y, lerp_speed);
            cam_y_bottom = utils::lerp(cam_y_bottom, target_cam_y_bottom, lerp_speed);
            cam_z_look_at_bottom_offset = utils::lerp(cam_z_look_at_bottom_offset, target_cam_z_look_at_bottom_offset, lerp_speed);


            NE_CameraSet(scene->camera_top,
                cam_x, cam_y, cam_z, // position
                cam_x, -3, cam_z - z_look_at_distance, // look at
                0, 1, 0); // up

            // real DS: look at Y -28, simulator: -21
            NE_CameraSet(scene->camera_bottom,
                cam_x, cam_y_bottom, cam_z + cam_z_look_at_bottom_offset, // position -- FIXME
                cam_x, -28, cam_z - z_look_at_distance + cam_z_look_at_bottom_offset, // look at
                0, 1, 0); // up
        }
    }


    // coins ---------------------------
    namespace coins {
        struct coin {

        };

        coin coins[10]; // max 10
    }


    // ground ---------------------------
    namespace ground {
        int ground_start_z = -10;

        void update() {
            // Update ground 
            if (ground_start_z < cameras::cam_z - 20) {
                ground_start_z += 10;
            }
        }

        void draw(SceneData* scene) {
            // draw ground
            NE_ModelScale(scene->ground, 7, 1, 1);

            for (int i = 0; i <= 5; i++) {
                NE_ModelSetCoord(scene->ground, 0, -3.1, ground_start_z + i * 13);
                NE_ModelDraw(scene->ground);
            }
        }
    }


    // tracks  ---------------------------
    namespace tracks {
        float track_height = -3;
        const int num_track_parts = 7;
        int track_gap = 7;
        int track_start_z = -10;

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

    // coins ---- 
    void draw_coin(SceneData* scene) {
        // coin 
        NE_ModelSetCoord(scene->coin, 0, -2, cameras::cam_z + 5);
        NE_ModelRotate(scene->coin, 0, 10, 0);
        NE_ModelDraw(scene->coin);
    }

    

    // DRAW the two screens ----------------------------
    void draw_3d_scene_top(void *arg) {
        SceneData *scene = (SceneData*) arg;

        NE_ClearColorSet(SKY_COLOR, 31, 63);

        NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);

        NE_CameraUse(scene->camera_top);

        // Draw ground, tracks and trains
        ground::draw(scene);        
        tracks::draw(scene);
        trains::draw(scene);

        // coin 
        draw_coin(scene);
    }

    void draw_3d_scene_bottom(void *arg) {
        SceneData *scene = (SceneData*) arg;

        NE_ClearColorSet(SKY_COLOR, 31, 63);

        NE_PolyFormat(31, 1, NE_LIGHT_0, NE_CULL_BACK, NE_FOG_ENABLE);

        NE_CameraUse(scene->camera_bottom);

        // Draw ground, tracks and trains
        ground::draw(scene);
        tracks::draw(scene);
        trains::draw(scene);

        // coin 
        draw_coin(scene);
}


    // INIT graphics objects ------------------------------
    void init_all(SceneData *scene) {
        // Allocate objects...
        scene->train = NE_ModelCreate(NE_Static);
        scene->track = NE_ModelCreate(NE_Static);
        scene->ground = NE_ModelCreate(NE_Static);
        scene->coin = NE_ModelCreate(NE_Static);

        scene->camera_top = NE_CameraCreate();
        scene->camera_bottom = NE_CameraCreate();

        // Load models
        NE_ModelLoadStaticMesh(scene->train, trein_bin);
        NE_ModelLoadStaticMesh(scene->track, track_bin);
        NE_ModelLoadStaticMesh(scene->ground, ground_bin);
        NE_ModelLoadStaticMesh(scene->coin, coin_bin);

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
        NE_ModelSetMaterial(scene->coin, material);

        // Set light color and direction
        NE_LightSet(0, NE_White, -0.5, -0.5, -0.5);

        // Enable fog
        u16 depth = 0x7EA0;
        u8 shift = 5;
        u8 mass = 2;
        u32 color = SKY_COLOR;

        NE_FogEnable(shift, color, 31, mass, depth);
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

        touchPosition touch;

        while (1) {
            // Move forward
            cameras::cam_z += speed;

            NE_WaitForVBL(NE_CAN_SKIP_VBL);

            // Draw 3D scenes
            NE_ProcessDualArg(draw_3d_scene_bottom, draw_3d_scene_top, &scene, &scene);

            // Refresh keys
            scanKeys();
            uint32_t keys = keysHeld();
            uint32_t kdown = keysDown();
            int keys_up = keysUp();

            touchRead(&touch);

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


            // Swipe detection ----------------
            if (kdown & KEY_TOUCH) {
                swipe_detection::start_touch_x = touch.px;
                swipe_detection::is_swiping = true;
            }

            if (keys & KEY_TOUCH && swipe_detection::is_swiping) {
                int distance = swipe_detection::start_touch_x - touch.px;
                if (distance < -20) {
                    swipe_detection::is_swiping = false;
                    if (current_lane >= 0) current_lane--;
                }
                if (distance > 20) {
                    swipe_detection::is_swiping = false;
                    if (current_lane < 2) current_lane++;
                }
            }

            if (!keys_up & KEY_TOUCH) {
                swipe_detection::is_swiping = false;
            }



            ground::update();
            tracks::update();
            trains::update(cameras::cam_z);
            cameras::update(&scene);
        }

        return 0;
    }

}