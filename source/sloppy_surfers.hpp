#pragma once

#include <NEMain.h>
#include <string>
#include <format> 

#include "trein_bin.h"
#include "coin_bin.h"
#include "pole_bin.h"
#include "robot_dsm_bin.h"
#include "robot_walk_dsa_bin.h"

#include "number0_bin.h"
#include "number1_bin.h"
#include "number2_bin.h"
#include "number3_bin.h"
#include "number4_bin.h"
#include "number5_bin.h"
#include "number6_bin.h"
#include "number7_bin.h"
#include "number8_bin.h"
#include "number9_bin.h"

#include "gameover_bin.h"
#include "sloppysurfers_bin.h"

#include "texture.h"


#define SKY_COLOR RGB15(18, 28, 31)

namespace sloppy_surfers {
    NE_Material *material;
    
    struct SceneData {
        NE_Camera *camera_top, *camera_bottom;
        NE_Model *train, *track, *pole, *ground, *coin, *player;
        NE_Model *numbers[10];
        NE_Model *gameover, *sloppysurfers;
    };


    // game -----------------------------------------------
    enum class GameState {
        Menu,
        Playing, 
        GameOver,
        Credits
    };

    int frame = 0;
    float speed = 0.075;
    int score = 0;
    int coins_collected = 0; // maybe?
    int high_score = 0;
    GameState game_state = GameState::Playing;


    


    // lanes
    float lane_gap = 2.5;
    int current_lane = 0; // -1 / 0 / 1


    // swipe detection ------------------------------------
    namespace swipe_detection {
        int start_touch_x = 0;
        int start_touch_y = 0;
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

            bool hit = false;
        };
        
        train trains[] = {
            train{.z = 50 + rand() % 20, .x = -lane_gap}, // left
            train{.z =  100 + rand() % 20, .x = 0}, // center
            train{.z =  150 + rand() % 20, .x = lane_gap} // right
        };

        void update(int cam_z) {
            for (train &t: trains) {
                if (!t.hit) {
                    t.z += -0.3 - speed / 4;
                }

                // respawn in the distance
                if (t.z < cam_z - 10) {
                    t.z = cam_z + 50 + rand() % 150;
                }
            }     
        }
        
        void draw(SceneData* scene)  {
            // draw train
            for (train &t: trains) {
                NE_ModelScale(scene->train, 1.3, 1.3, 1.3);
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
            if (game_state == GameState::Playing && train_in_lane.z && (train_in_lane.z < cam_z + 20)) { // high camera
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
            float lerp_speed = 0.1; //utils::map(speed, 0.0, 3.0, 0.1, 0.3); // camera moves faster when speed increases
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


    // player ---------------------------
    namespace player {
        NE_Animation *walk;

        float target_x = 0;
        float x = 0;
        float y = -2.5;
        float z = 0;

        const float scale = 0.15;

        void update() {
            target_x = (float) current_lane * lane_gap;
            x = utils::lerp(x, target_x, 0.2);
            z = cameras::cam_z + 2.5;

            for (trains::train &t: trains::trains) {
                if (abs(t.x - x) < 1 && abs(t.z - z) < 8.25) {
                    t.hit = true;
                    game_state = GameState::GameOver;
                }
            }
        }

        void draw(SceneData* scene) {
            NE_ModelSetRot(scene->player, 0, 130, 0);
            NE_ModelScale(scene->player, scale, scale, scale);
            NE_ModelSetCoord(scene->player, x, y, z);
            NE_ModelDraw(scene->player);
        }
    }


    // coins ---------------------------
    namespace coins {
        int rotation = 0;

        struct coin {
            float x, y, z;
            bool is_collecting = false;
            float scale = 1;
        };

        std::vector<coin> coins; // max 10

        float coin_count_x, coin_count_y, coin_count_z;

        float coin_count_rotation = 0;
        float coin_count_rotation_target = 0;

        float coin_count_scale = 0.4;
        float coin_count_scale_target = 0.4;


        void update() {
            if (frame % 45 == 0) {
                coin new_coin = coin();
                new_coin.x = (rand() % 3 - 1) * lane_gap;
                new_coin.y = -2;
                new_coin.z = cameras::cam_z + 50;

                coins.push_back(new_coin);
            }

            coin_count_x = cameras::cam_x + 1;
            coin_count_y = cameras::cam_y_bottom - 1.5;
            coin_count_z = cameras::cam_z + cameras::cam_z_look_at_bottom_offset - .15;
            // coin_count_rotation = utils::lerp(coin_count_rotation, coin_count_rotation_target, 0.3);
            coin_count_scale = utils::lerp(coin_count_scale, coin_count_scale_target, 0.2);

            for (int i = 0; i < coins.size(); i++) {
                coin *c = &coins.at(i);
                if (c->z < cameras::cam_z - 10) {
                    coins.erase(coins.begin() + i);
                }

                if ((abs(c->x - player::x) + abs(c->z - player::z)) < 1 && !c->is_collecting) {
                    coins_collected++;
                    c->is_collecting = true;
                    // coin_count_rotation_target = 800;
                    coin_count_scale = .8;
                }

                if (c->is_collecting) {
                    c->x = utils::lerp(c->x, coin_count_x, 0.05);
                    c->y = utils::lerp(c->y, coin_count_y, 0.05);
                    c->z = utils::lerp(c->z, coin_count_z, 0.05);
                    c->scale = utils::lerp(c->scale, 0, 0.03);

                    if (c->scale < 0.5) {
                        coin_count_scale_target = 0.4;
                    }

                    if (c->scale < 0.2) {
                        coins.erase(coins.begin() + i);
                    }
                }
            }   
        }

        void draw(SceneData* scene) {
            rotation += 10;
            NE_ModelSetRot(scene->coin, 0, rotation, 0);

            for (coin &c: coins) {
                NE_ModelScale(scene->coin, c.scale, c.scale, c.scale);
                NE_ModelSetCoord(scene->coin, c.x, c.y, c.z);
                NE_ModelDraw(scene->coin);
            }
        }

        void draw_coin_count(SceneData* scene) {
            // the coin
            NE_ModelSetCoord(scene->coin, cameras::cam_x + 1, cameras::cam_y_bottom - 1.5, cameras::cam_z + cameras::cam_z_look_at_bottom_offset - .15);
            NE_ModelSetRot(scene->coin, -130, coin_count_rotation, 0);
            NE_ModelScale(scene->coin, coin_count_scale, coin_count_scale, coin_count_scale);
            NE_ModelDraw(scene->coin);

            // the score
            std::string coin_count_str = std::format("{}", coins_collected);
            for (int i = 0; i < coin_count_str.size(); i++) {
                int number = std::stoi(coin_count_str.substr(i, 1));
                NE_Model* number_to_draw = scene->numbers[number];
                NE_ModelSetCoord(number_to_draw, cameras::cam_x + .6 - i * 0.2, cameras::cam_y_bottom - 1.5, cameras::cam_z + cameras::cam_z_look_at_bottom_offset - .15);
                NE_ModelSetRot(number_to_draw, 130, 0, 0);
                NE_ModelScale(number_to_draw, 0.1, 0.1, 0.1);
                NE_ModelDraw(number_to_draw);
            }
}
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

        int pole_start_z = -10;
        int pole_gap = 28;
        const int num_pole_parts = 7;

        void update() {
            if (track_start_z < cameras::cam_z - 10) {
                track_start_z += 7;
            }
            
            if (pole_start_z < cameras::cam_z - 28) {
                pole_start_z += 28;
            }
        }

        void draw(SceneData* scene) {
            for (int pole_i = 0; pole_i < num_pole_parts; ++pole_i) {
                // draw tracks
                NE_ModelSetCoord(scene->pole, 0, -3, pole_start_z + pole_i * pole_gap);
                NE_ModelDraw(scene->pole);
            }

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

    namespace menu {
        void draw(SceneData* scene) {
            if (game_state == GameState::GameOver) {
                NE_ModelSetCoord(scene->gameover, cameras::cam_x, 0, cameras::cam_z + 8);
                NE_ModelDraw(scene->gameover);
            }
        }
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
        coins::draw(scene);
        player::draw(scene);
        menu::draw(scene);
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
        coins::draw(scene);
        player::draw(scene);

        // UI
        coins::draw_coin_count(scene);
}


    // INIT graphics objects ------------------------------
    void init_all(SceneData *scene) {
        // Allocate objects...
        scene->train = NE_ModelCreate(NE_Static);
        scene->track = NE_ModelCreate(NE_Static);
        scene->pole = NE_ModelCreate(NE_Static);
        scene->ground = NE_ModelCreate(NE_Static);
        scene->coin = NE_ModelCreate(NE_Static);
        scene->gameover = NE_ModelCreate(NE_Static);
        scene->player = NE_ModelCreate(NE_Animated);

        scene->camera_top = NE_CameraCreate();
        scene->camera_bottom = NE_CameraCreate();

        // Load models
        NE_ModelLoadStaticMesh(scene->train, trein_bin);
        NE_ModelLoadStaticMesh(scene->track, track_bin);
        NE_ModelLoadStaticMesh(scene->pole, pole_bin);
        NE_ModelLoadStaticMesh(scene->ground, ground_bin);
        NE_ModelLoadStaticMesh(scene->coin, coin_bin);
        NE_ModelLoadStaticMesh(scene->gameover, gameover_bin);

        // Numbers
        for (int i = 0; i < 10; i++) {
            scene->numbers[i] = NE_ModelCreate(NE_Static);
        }
        NE_ModelLoadStaticMesh(scene->numbers[0], number0_bin);
        NE_ModelLoadStaticMesh(scene->numbers[1], number1_bin);
        NE_ModelLoadStaticMesh(scene->numbers[2], number2_bin);
        NE_ModelLoadStaticMesh(scene->numbers[3], number3_bin);
        NE_ModelLoadStaticMesh(scene->numbers[4], number4_bin);
        NE_ModelLoadStaticMesh(scene->numbers[5], number5_bin);
        NE_ModelLoadStaticMesh(scene->numbers[6], number6_bin);
        NE_ModelLoadStaticMesh(scene->numbers[7], number7_bin);
        NE_ModelLoadStaticMesh(scene->numbers[8], number8_bin);
        NE_ModelLoadStaticMesh(scene->numbers[9], number9_bin);

        // Load player model and animation
        player::walk = NE_AnimationCreate();
        NE_ModelLoadDSM(scene->player, robot_dsm_bin);
        NE_AnimationLoad(player::walk, robot_walk_dsa_bin);
        NE_ModelSetAnimation(scene->player, player::walk);
        NE_ModelAnimStart(scene->player, NE_ANIM_LOOP, floattof32(0.25));


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
        NE_ModelSetMaterial(scene->pole, material);
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
            NE_WaitForVBL(NE_UPDATE_ANIMATIONS);
            // Draw 3D scenes
            NE_ProcessDualArg(draw_3d_scene_bottom, draw_3d_scene_top, &scene, &scene);


            // Refresh keys and touch
            scanKeys();
            uint32_t keys = keysHeld();
            uint32_t kdown = keysDown();
            int keys_up = keysUp();

            touchRead(&touch);



            if (game_state == GameState::Playing) {
                // Move forward
                frame++;
                cameras::cam_z += speed;
                speed += 0.00001;

                // Speed and lane switching
                // if (keys & KEY_UP) {
                //     speed += 0.01;
                // }
                // if (keys & KEY_DOWN) {
                //     speed -= 0.01;
                // }
                if (kdown & KEY_LEFT) {
                    // NE_ModelRotate(scene.train, 0, 2, 0);
                    if (current_lane < 1) current_lane++;
                }
                if (kdown & KEY_RIGHT) {
                    // NE_ModelRotate(scene.train, 0, -2, 0);
                    if (current_lane >= 0) current_lane--;
                }

                // Swipe detection -------------------
                if (kdown & KEY_TOUCH) {
                    swipe_detection::start_touch_x = touch.px;
                    swipe_detection::start_touch_y = touch.py;
                    swipe_detection::is_swiping = true;
                }

                if (keys & KEY_TOUCH && swipe_detection::is_swiping) {
                    int distance_x = swipe_detection::start_touch_x - touch.px;
                    int distance_y = swipe_detection::start_touch_y - touch.py;
                    if (distance_x < -10) {
                        swipe_detection::is_swiping = false;
                        if (current_lane >= 0) current_lane--;
                    }
                    else if (distance_x > 10) {
                        swipe_detection::is_swiping = false;
                        if (current_lane < 1) current_lane++;
                    }
                    else if (distance_y < -20) {
                        swipe_detection::is_swiping = false;
                        if (current_lane < 2) current_lane++;
                    }
                }

                if (!keys_up & KEY_TOUCH) {
                    swipe_detection::is_swiping = false;
                }
            }
            

            
            // Update everything
            ground::update();
            tracks::update();
            trains::update(cameras::cam_z);
            coins::update();
            player::update();
            cameras::update(&scene);
        }

        return 0;
    }

}