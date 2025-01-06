#pragma once

#include <NEMain.h>


// Global material
NE_Material *material_mp3;
NE_Material *text_material;


enum class scene {
    start,
    game,
    gameover
};


// The time, also used for score/distance
double t;


// camera
float cam_y = 0.0;
float cam_z = -10.0;
float cam_x = 0.0;

float target_cam_y = cam_y;
float target_cam_x = cam_x;
float target_cam_z = cam_z;

// states
bool trein_active = true;
bool storm_active = true;

float speed = 0.2;
scene current_scene = scene::start;



struct scene_data {
    NE_Camera *camera;

    NE_Model *heart;
    int heart_poly_id;
    NE_Model *mp3;
    NE_Model *swf;
    NE_Model *trein;
    int mp3_poly_id;
    int trein_poly_id;

    NE_Model *track;
    int track_poly_id;

    NE_Model *ground;
    int ground_poly_id;

    NE_Model *sloppy_text;



    NE_Model *wout_head;
    int wout_head_poly_id;
};