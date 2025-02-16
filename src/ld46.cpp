/**
 * @file ld46.cpp
 * @date 18-Apr-2020
 */

#define __SCROLL_IMPL__
#include "ld46.h"
#undef __SCROLL_IMPL__

#include "Object.h"
#include "Train.h"
#include "Player.h"

#include "orxExtensions.h"

#ifdef __orxMSVC__

 /* Requesting high performance dedicated GPU on hybrid laptops */
__declspec(dllexport) unsigned long NvOptimusEnablement        = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#endif // __orxMSVC__

static orxBOOL sbRestart = orxTRUE;

orxBOOL orxFASTCALL SaveCallback(const orxSTRING _zSectionName, const orxSTRING _zKeyName, const orxSTRING _zFileName, orxBOOL _bUseEncryption)
{
    return (!orxString_Compare(_zSectionName, "Save")) ? orxTRUE : orxFALSE;
}

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
                    orxConfig_PushSection("Runtime");

                    orxOBJECT *pstTrain = orxOBJECT(orxStructure_Get(orxConfig_GetU64(orxSpawner_GetName(pstSpawner))));
                    if(pstTrain)
                    {
                        const orxSTRING zTrain = orxObject_GetName(pstTrain);

                        if(zTrain != orxSTRING_EMPTY)
                        {
                            static orxU32 su32Count = 0;
                            const orxSTRING zDifficulty;

                            orxConfig_PushSection("Game");
                            if(!su32Count)
                            {
                                zDifficulty = "Hard";
                                su32Count = orxConfig_GetU32("HardDoorSkip");
                            }
                            else
                            {
                                zDifficulty = "Easy";
                                su32Count--;
                            }
                            orxConfig_PopSection();

                            orxConfig_PushSection(zTrain);
                            const orxSTRING zType = orxConfig_GetBool("RightDoor") ? "Open" : "Close";
                            orxConfig_PopSection();

                            orxCHAR acName[32] = {};
                            orxString_NPrint(acName, sizeof(acName) - 1, "%s%sDoor", zDifficulty, zType);
                            const orxSTRING zNewTrain = orxConfig_GetString(acName);

                            orxConfig_PushSection(orxSpawner_GetName(pstSpawner));
                            orxConfig_SetString("Object", zNewTrain);
                            orxConfig_PopSection();
                        }
                    }

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
                            orxOBJECT *pstPreviousTrain = orxOBJECT(orxStructure_Get(orxConfig_GetU64(orxSpawner_GetName(pstSpawner))));
                            if(pstPreviousTrain)
                            {
                                Train *poTrain = (Train *)orxObject_GetUserData(pstObject);
                                poTrain->u64PreviousGUID = orxStructure_GetGUID(pstPreviousTrain);
                            }
                            orxConfig_SetU64(orxSpawner_GetName(pstSpawner), orxStructure_GetGUID(pstObject));
                            orxConfig_PopSection();
                        }
                    }
                    else if(!orxString_Compare(orxObject_GetName(pstObject), "DashSmoke"))
                    {
                        orxSPAWNER *pstSpawner = orxSPAWNER(orxObject_GetOwner(pstObject));

                        if(pstSpawner)
                        {
                            orxOBJECT *pstPlayer = orxOBJECT(orxSpawner_GetParent(pstSpawner));

                            if(pstPlayer)
                            {
                                orxOBJECT *pstTrain = orxOBJECT(orxObject_GetParent(pstPlayer));

                                if(pstTrain)
                                {
                                    orxObject_Attach(pstObject, pstTrain);
                                }
                            }
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
    if(orxInput_HasBeenActivated("Quit"))
    {
        // Should restart?
        sbRestart = orxInput_HasBeenActivated("Reset");

        // Send close event
        orxEvent_SendShort(orxEVENT_TYPE_SYSTEM, orxSYSTEM_EVENT_CLOSE);
    }
    // Screenshot?
    else if(orxInput_HasBeenActivated("Screenshot"))
    {
        orxScreenshot_Capture();
    }
    // Menu?
    else if(orxInput_HasBeenActivated("Menu"))
    {
        orxConfig_PushSection("Runtime");
        if(!orxOBJECT(orxStructure_Get(orxConfig_GetU64("Menu")))
        && !orxOBJECT(orxStructure_Get(orxConfig_GetU64("Tutorial"))))
        {
            PauseGame(orxFALSE);
            for(ScrollObject *poObject = GetNextObject();
                poObject;
                poObject = GetNextObject())
            {
                DeleteObject(poObject);
            }

            orxConfig_PushSection("Runtime");
            orxConfig_SetU32("Player1Score", 0);
            orxConfig_SetU32("Player2Score", 0);
            orxConfig_SetU32("Player3Score", 0);
            orxConfig_SetU32("Player4Score", 0);
            orxConfig_PopSection();
            CreateObject("Menu");
        }
        orxConfig_PopSection();
    }
    else if(orxInput_HasBeenActivated("Reset"))
    {
        PauseGame(orxFALSE);
        for(ScrollObject *poObject = GetNextObject();
            poObject;
            poObject = GetNextObject())
        {
            DeleteObject(poObject);
        }
        orxConfig_PushSection("Runtime");
        if(!orxString_Compare(orxConfig_GetString("Players"), "1P"))
        {
            orxConfig_SetU32("Player1Score", 0);
        }
        orxConfig_PopSection();
        CreateObject("Scene");
    }
    else
    {
        orxConfig_PushSection("Runtime");

        if(!orxOBJECT(orxStructure_Get(orxConfig_GetU64("GameOver"))))
        {
            // Pause?
            if(!orxOBJECT(orxStructure_Get(orxConfig_GetU64("Menu")))
            && !orxOBJECT(orxStructure_Get(orxConfig_GetU64("Title"))))
            {
                if(orxInput_HasBeenActivated("Pause"))
                {
                    PauseGame(!IsGamePaused());
                    if(IsGamePaused())
                    {
                        CreateObject("Pause");
                    }
                    else
                    {
                        ScrollObject *poPause = GetObject(orxConfig_GetU64("Pause"));
                        if(poPause)
                        {
                            DeleteObject(poPause);
                        }
                    }
                }
            }

            // Train fixup
            for(Train *poTrain = GetNextObject<Train>();
                poTrain;
                poTrain = GetNextObject<Train>(poTrain))
            {
                orxOBJECT *pstPrevious = orxOBJECT(orxStructure_Get(poTrain->u64PreviousGUID));
                if(pstPrevious && (orxObject_GetLifeTime(pstPrevious) > orxFLOAT_0))
                {
                    orxVECTOR vNewPos, vPos, vSize, vScale;
                    orxVector_Mul(&vSize, orxObject_GetSize(pstPrevious, &vSize), orxObject_GetScale(pstPrevious, &vScale));
                    orxObject_GetPosition(pstPrevious, &vPos);
                    poTrain->GetPosition(vNewPos);
                    if(orxMath_Abs(vPos.fX - vNewPos.fX) > vSize.fX - orx2F(0.5f))
                    {
                          vNewPos.fX = vPos.fX + vSize.fX - orx2F(0.51f);
                          poTrain->SetPosition(vNewPos);
                    }
                }
            }

            // Game over / Save?
            orxConfig_PushSection("Save");
            orxU32 u32HighScore = orxConfig_GetU32("HighScore");
            orxConfig_PopSection();
            orxU32 u32Alive = 0, u32Dead = 0;
            Player *poAlive = orxNULL;
            for(Player *poPlayer = ld46::GetInstance().GetNextObject<Player>();
                poPlayer;
                poPlayer = ld46::GetInstance().GetNextObject<Player>(poPlayer))
            {
                if(!poPlayer->IsDead())
                {
                    u32Alive++;
                    poAlive = poPlayer;
                }
                else
                {
                    u32Dead++;
                }

                orxCHAR acName[32] = {};
                orxString_NPrint(acName, sizeof(acName) - 1, "%sScore", poPlayer->GetName());
                orxU32 u32Score = orxConfig_GetU32(acName);
                if(u32Score > u32HighScore)
                {
                    u32HighScore = u32Score;
                }
            }
            if(u32Alive + u32Dead == 1)
            {
                orxConfig_PushSection("Save");
                orxConfig_SetU32("HighScore", u32HighScore);
                orxConfig_PopSection();
            }

            if((u32Dead > 0) && (u32Alive <= 1))
            {
                ld46::GetInstance().CreateObject("GameOver");
                if(poAlive)
                {
                    orxConfig_SetU64("Winner", poAlive->GetGUID());
                }
            }
        }
        else if(orxInput_HasBeenActivated("Pause"))
        {
            PauseGame(orxTRUE);
        }

        orxConfig_PopSection();
    }
}

/** Init function, it is called when all orx's modules have been initialized
 */
orxSTATUS ld46::Init()
{
    // Init extensions
    InitExtensions();

    // Deactivate object debug messages
    orxDEBUG_ENABLE_LEVEL(orxDEBUG_LEVEL_OBJECT, orxFALSE);

    // Initialize random seeds
    orxMath_InitRandom(orxF2U(orxSystem_GetSystemTime() * 1000.0f));

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
            orxTexture_Load(orxConfig_GetString("Texture"), orxFALSE);
        }

        // Has left door?
        if(orxConfig_HasValue("LeftDoor"))
        {
            // Stores it
            orxCHAR acName[32] = {};
            const orxSTRING zTrain = orxConfig_GetCurrentSection();
            const orxSTRING zDifficulty = (orxConfig_HasValue("RightDoor") && !orxConfig_GetBool("RightDoor")) ? "Hard" : "Easy";
            const orxSTRING zType = orxConfig_GetBool("LeftDoor") ? "Open" : "Close";
            orxString_NPrint(acName, sizeof(acName) - 1, "%s%sDoor", zDifficulty, zType);
            orxConfig_PushSection("Runtime");
            orxConfig_AppendListString(acName, &zTrain, 1);
            orxConfig_PopSection();

        }

        // Pops it
        orxConfig_PopSection();
    }

    // Register event handler
    orxEvent_AddHandler(orxEVENT_TYPE_SPAWNER, &EventHandler);

    // Go to title
    CreateObject("Title");

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
    // Exit from extensions
    ExitExtensions();

    // Save highscores
    orxConfig_Save(orxFile_GetApplicationSaveDirectory("RailroadLantern/score.dat"), orxTRUE, &SaveCallback);
}

/** BindObjects function, ScrollObject-derived classes are bound to config sections here
 */
void ld46::BindObjects()
{
    // Bind the classes
    ScrollBindObject<Object>("Object");
    ScrollBindObject<Train>("Train");
    ScrollBindObject<Player>("Player");
}

/** Bootstrap function, it is called before config is initialized, allowing for early resource storage definitions
 */
orxSTATUS ld46::Bootstrap() const
{
    // Bootstrap extensions
    BootstrapExtensions();

    // Loads scores
    orxConfig_Load(orxFile_GetApplicationSaveDirectory("RailroadLantern/score.dat"));

    // Return orxSTATUS_FAILURE to prevent orx from loading the default config file
    return orxSTATUS_SUCCESS;
}

/** Main function
 */
int main(int argc, char **argv)
{
  // Should restart?
  while(sbRestart)
  {
    // Clears restart
    sbRestart = orxFALSE;

    // Execute our game
    ld46::GetInstance().Execute(argc, argv);
  }

    // Done!
    return EXIT_SUCCESS;
}
