/**
 * @file ld46.cpp
 * @date 18-Apr-2020
 */

#define __SCROLL_IMPL__
#include "ld46.h"
#undef __SCROLL_IMPL__

#include "Object.h"
#include "Player.h"

#define orxIMGUI_HEADER_ONLY
#include "orxImGui.cpp"
#undef orxIMGUI_HEADER_ONLY

#define orxARCHIVE_HEADER_ONLY
#include "orxArchive.cpp"
#undef orxARCHIVE_HEADER_ONLY

/** Update function, it has been registered to be called every tick of the core clock
 */
void ld46::Update(const orxCLOCK_INFO &_rstInfo)
{
    // Should quit?
    if(orxInput_IsActive("Quit"))
    {
        // Send close event
        orxEvent_SendShort(orxEVENT_TYPE_SYSTEM, orxSYSTEM_EVENT_CLOSE);
    }
}

/** Init function, it is called when all orx's modules have been initialized
 */
orxSTATUS ld46::Init()
{
    // Initialize Dear ImGui
    orxImGui_Init();

    // Initialize archive (ZIP) resource type
    orxArchive_Init();

    // Pushes game section
    orxConfig_PushSection("Game");

    // Creates all viewports
    for(orxS32 i = 0, iCount = orxConfig_GetListCount("ViewportList"); i < iCount; i++)
    {
        orxViewport_CreateFromConfig(orxConfig_GetListString("ViewportList", i));
    }

    // Create the scene
    CreateObject("Scene");

    // Done!
    return orxSTATUS_SUCCESS;
}

/** Run function, it should not contain any game logic
 */
orxSTATUS ld46::Run()
{
    // Return orxSTATUS_FAILURE to instruct orx to quit
    return orxSTATUS_SUCCESS;
}

/** Exit function, it is called before exiting from orx
 */
void ld46::Exit()
{
    // Exits from Dear ImGui
    orxImGui_Exit();

    // Let Orx clean all our mess automatically. :)
}

/** BindObjects function, ScrollObject-derived classes are bound to config sections here
 */
void ld46::BindObjects()
{
    // Bind the Object class to the Object config section
    ScrollBindObject<Object>("Object");
    ScrollBindObject<Player>("Player");
}

/** Bootstrap function, it is called before config is initialized, allowing for early resource storage definitions
 */
orxSTATUS ld46::Bootstrap() const
{
    // Add a config storage to find the initial config file
    orxResource_AddStorage(orxCONFIG_KZ_RESOURCE_GROUP, "../data/config", orxFALSE);

    // Return orxSTATUS_FAILURE to prevent orx from loading the default config file
    return orxSTATUS_SUCCESS;
}

/** Main function
 */
int main(int argc, char **argv)
{
    // Execute our game
    ld46::GetInstance().Execute(argc, argv);

    // Done!
    return EXIT_SUCCESS;
}