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

orxSTATUS orxFASTCALL EventHandler(const orxEVENT *_pstEvent)
{
    switch(_pstEvent->eType)
    {
        case orxEVENT_TYPE_SPAWNER:
        {
            orxSPAWNER *pstSpawner = orxSPAWNER(_pstEvent->hSender);

            switch(_pstEvent->eID)
            {
                case orxSPAWNER_EVENT_WAVE_START:
                {
                    orxBOOL bOpen = orxTRUE;

                    orxConfig_PushSection("Runtime");
                    const orxSTRING zTrain = orxConfig_GetString(orxSpawner_GetName(pstSpawner));

                    if(zTrain != orxSTRING_EMPTY)
                    {
                        orxConfig_PushSection(zTrain);
                        bOpen = orxConfig_GetBool("RightDoor");
                        orxConfig_PopSection();
                    }

                    const orxSTRING zNewTrain = orxConfig_GetString(bOpen ? "OpenDoor" : "CloseDoor");

                    orxConfig_PushSection(orxSpawner_GetName(pstSpawner));
                    orxConfig_SetString("Object", zNewTrain);
                    orxConfig_PopSection();

                    orxConfig_PopSection();

                    break;
                }
                case orxSPAWNER_EVENT_SPAWN:
                {
                    orxOBJECT *pstObject = orxOBJECT(_pstEvent->hRecipient);

                    if(orxString_SearchString(orxObject_GetName(pstObject), "Train"))
                    {
                       orxSPAWNER *pstSpawner = orxSPAWNER(orxObject_GetOwner(pstObject));

                       if(pstSpawner)
                       {
                           orxConfig_PushSection("Runtime");
                           orxConfig_SetString(orxSpawner_GetName(pstSpawner), orxObject_GetName(pstObject));
                           orxConfig_PopSection();
                       }
                    }

                    break;
                }
            }

            break;
        }
    }

    return orxSTATUS_SUCCESS;
}

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
    // Deactivate object debug messages
    orxDEBUG_ENABLE_LEVEL(orxDEBUG_LEVEL_OBJECT, orxFALSE);

    // Initialize random seeds
    orxMath_InitRandom(orxF2U(orxSystem_GetSystemTime() * 1000.0f));

    // Initialize Dear ImGui
    orxImGui_Init();

    // Pushes game section
    orxConfig_PushSection("Game");

    // Creates all viewports
    for(orxS32 i = 0, iCount = orxConfig_GetListCount("ViewportList"); i < iCount; i++)
    {
        orxViewport_CreateFromConfig(orxConfig_GetListString("ViewportList", i));
    }

    // For all sections
    for(orxU32 i = 0, iCount = orxConfig_GetSectionCount(); i < iCount; i++)
    {
        // Pushes it
        orxConfig_PushSection(orxConfig_GetSection(i));

        // Has texture?
        if(orxConfig_HasValue("Texture"))
        {
            // PreLoads it
            orxTexture_CreateFromFile(orxConfig_GetString("Texture"), orxFALSE);
        }

        // Has left door?
        if(orxConfig_HasValue("LeftDoor"))
        {
            // Stores it
            const orxSTRING zName = orxConfig_GetCurrentSection();
            const orxSTRING zSection = orxConfig_GetBool("LeftDoor") ? "OpenDoor" : "CloseDoor";
            orxConfig_PushSection("Runtime");
            orxConfig_AppendListString(zSection, &zName, 1);
            orxConfig_PopSection();
        }

        // Pops it
        orxConfig_PopSection();
    }

    // Register event handler
    orxEvent_AddHandler(orxEVENT_TYPE_SPAWNER, &EventHandler);

    // Create the menu
    CreateObject("Menu");

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
    // Initialize archive (ZIP) resource type
    orxArchive_Init();

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
