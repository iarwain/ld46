/**
 * @file Player.cpp
 * @date 18-Apr-2020
 */

#include "Player.h"

void orxFASTCALL Player::ResetDash(const orxCLOCK_INFO *_pstClockInfo, void *_pContext)
{
    Player *poPlayer = (Player *)_pContext;
    poPlayer->bIsDashing = orxFALSE;
    poPlayer->SetSpeed(orxVECTOR_0);
}

void Player::OnCreate()
{
    Object::OnCreate();
    orxConfig_SetBool("IsPlayer", orxTRUE);

    // Enables its inputs
    orxInput_EnableSet(orxConfig_GetString("Input"), orxTRUE);

    // Inits lamp
    orxFLOAT fLampCapacity = orxConfig_GetFloat("LampCapacity");
    orxCHAR acName[32] = {};
    orxString_NPrint(acName, sizeof(acName) - 1, "%sOil", GetModelName());
    orxConfig_PushSection("Runtime");
    orxConfig_SetFloat(acName, fLampCapacity);
    orxConfig_PopSection();
}

void Player::OnDelete()
{
    Object::OnDelete();
}

void Player::Update(const orxCLOCK_INFO &_rstInfo)
{
    PushConfigSection();

    Object::Update(_rstInfo);

    orxFLOAT fLampBurnRate = orxConfig_GetFloat("LampBurnRate");
    orxCHAR acName[32] = {};
    orxString_NPrint(acName, sizeof(acName) - 1, "%sOil", GetModelName());
    orxConfig_PushSection("Runtime");
    orxFLOAT fLampOil = orxConfig_GetFloat(acName);
    fLampOil = orxMAX(fLampOil - fLampBurnRate * _rstInfo.fDT, orxFLOAT_0);
    orxConfig_SetFloat(acName, fLampOil);
    orxConfig_PopSection();

    if(fLampOil == orxFLOAT_0)
    {
        //! Game Over
    }
    else
    {
        orxVECTOR vScale = {fLampOil, fLampOil, orxFLOAT_0};

        GetOwnedChild()->SetScale(vScale);

        const orxSTRING zSet = orxInput_GetCurrentSet();
        orxInput_SelectSet(orxConfig_GetString("Input"));

        // Dash
        if(!bIsDashing)
        {
            // Move
            orxVECTOR vSpeed = {orxInput_GetValue("MoveRight") - orxInput_GetValue("MoveLeft"), orxInput_GetValue("MoveDown") - orxInput_GetValue("MoveUp"), orxFLOAT_0};

            // Dash
            if(orxInput_HasBeenActivated("Dash"))
            {
                orxVector_Mulf(&vSpeed, &vSpeed, orxConfig_GetFloat("DashSpeed"));
                bIsDashing = orxTRUE;
                orxClock_AddGlobalTimer(ResetDash, orxConfig_GetFloat("DashDuration"), 1, this);
            }
            else
            {
                orxVector_Mulf(&vSpeed, &vSpeed, orxConfig_GetFloat("Speed"));
            }

            // Set speed
            SetSpeed(vSpeed);
        }

        orxInput_SelectSet(zSet);
    }

    PopConfigSection();
}

orxBOOL Player::OnCollide(ScrollObject *_poCollider, const orxSTRING _zPartName, const orxSTRING _zColliderPartName, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal)
{
    if(orxString_SearchString(_zColliderPartName, "Oil"))
    {
        orxConfig_PushSection("Runtime");
        orxConfig_SetU64("Collider", GetGUID());
        orxConfig_PopSection();
        _poCollider->AddConditionalTrack("PickUp");
    }

    return orxTRUE;
}
