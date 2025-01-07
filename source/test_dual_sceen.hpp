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

    

    void Draw3DSceneTop(void *arg)
    {
        SceneData *Scene = (SceneData*) arg;

        NE_ClearColorSet(NE_Green, 31, 63);

        NE_CameraUse(Scene->CameraTop);
        NE_ModelDraw(Scene->Teapot);
    }

    void Draw3DSceneBottom(void *arg)
    {
        SceneData *Scene = (SceneData*) arg;

        NE_ClearColorSet(NE_Red, 31, 63);

        NE_CameraUse(Scene->CameraBottom);
        NE_ModelDraw(Scene->Teapot);
    }

    void init_all(SceneData *Scene)
    {
        // Allocate objects...
        Scene->Teapot = NE_ModelCreate(NE_Static);
        Scene->Sphere = NE_ModelCreate(NE_Static);
        Scene->CameraTop = NE_CameraCreate();
        Scene->CameraBottom = NE_CameraCreate();

        // Setup camera
        float z_distance = -10;
        NE_CameraSet(Scene->CameraTop,
                    0, 2, z_distance,
                    0, -20, 0,
                    0, 1, 0);
        
        NE_CameraSet(Scene->CameraBottom,
                    0, 2.5, z_distance,
                    0, 0, 0,
                    0, 1, 0);

        // Load models
        NE_ModelLoadStaticMesh(Scene->Teapot, trein_bin);
        NE_ModelLoadStaticMesh(Scene->Sphere, trein_bin);

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
        NE_InitDual3D();
        NE_InitConsole();

        init_all(&Scene);

        bool console = true;

        while (1)
        {
            // NE_WaitForVBL();

            // Draw 3D scenes
            NE_ProcessDualArg(Draw3DSceneBottom, Draw3DSceneTop, &Scene, &Scene);

            // Refresh keys
            scanKeys();
            uint32_t keys = keysHeld();
            uint32_t kdown = keysDown();

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
        }

        return 0;
    }

}