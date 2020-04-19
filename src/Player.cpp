/**
 * @file Player.cpp
 * @date 18-Apr-2020
 */

#include "Player.h"

const orxSTRING Player::GetConfigVar(const orxSTRING _zVar) const
{
    static orxCHAR sacName[32] = {};

    orxString_NPrint(sacName, sizeof(sacName) - 1, "%s%s", GetModelName(), _zVar);

    return sacName;
}

void orxFASTCALL Player::ResetDash(const orxCLOCK_INFO *_pstClockInfo, void *_pContext)
{
    Player *poPlayer = (Player *)_pContext;
    poPlayer->bIsDashing = orxFALSE;
    poPlayer->SetSpeed(orxVECTOR_0);
}

void orxFASTCALL Player::UpdateBurnRate(const orxCLOCK_INFO *_pstClockInfo, void *_pContext)
{
    Player *poPlayer = (Player *)_pContext;
    if(!poPlayer->bIsDead)
    {
        poPlayer->PushConfigSection();
        poPlayer->u32BurnRateIndex = orxMIN((orxU32)orxConfig_GetListCount("LampBurnRate"), poPlayer->u32BurnRateIndex + 1);
        orxClock_AddGlobalTimer(UpdateBurnRate, orxConfig_GetFloat("LampCapacity") / orxConfig_GetListFloat("LampBurnRate", poPlayer->u32BurnRateIndex), 1, poPlayer);
        poPlayer->PopConfigSection();
    }
}

void Player::OnCreate()
{
    Object::OnCreate();
    orxConfig_SetBool("IsPlayer", orxTRUE);

    // Enables its inputs
    orxInput_EnableSet(orxConfig_GetString("Input"), orxTRUE);

    // Inits lamp
    orxFLOAT fLampCapacity = orxConfig_GetFloat("LampCapacity");
    orxConfig_PushSection("Runtime");
    orxConfig_SetFloat(GetConfigVar("Oil"), fLampCapacity);
    orxConfig_PopSection();

    // Inits score
    orxConfig_PushSection("Runtime");
    orxConfig_SetU32(GetConfigVar("Score"), 0);
    orxConfig_PopSection();

    // Inits burn rate
    orxClock_AddGlobalTimer(UpdateBurnRate, orxConfig_GetFloat("LampCapacity") / orxConfig_GetListFloat("LampBurnRate", u32BurnRateIndex), 1, this);
}

void Player::OnDelete()
{
    Object::OnDelete();

    // Removes all timers
    orxClock_RemoveGlobalTimer(orxNULL, orx2F(-1.0f), this);
}

void Player::Update(const orxCLOCK_INFO &_rstInfo)
{
    // Follow the train
    orxVECTOR vPos;
    GetPosition(vPos, orxTRUE);
    vPos.fZ -= orxFLOAT_1;
    ScrollObject *poTrain = ld46::GetInstance().PickObject(vPos, orxString_ToCRC("Train"));
    if(poTrain)
    {
        if(orxObject_GetParent(GetOrxObject()) != (orxSTRUCTURE *)poTrain->GetOrxObject())
        {
            orxObject_Attach(GetOrxObject(), poTrain->GetOrxObject());
        }
    }
    else
    {
        orxObject_Detach(GetOrxObject());
    }

    // Not dead?
    if(!bIsDead)
    {
        PushConfigSection();

        // Update base class
        Object::Update(_rstInfo);

        // Update lamp
        orxFLOAT fLampBurnRate = orxConfig_GetListFloat("LampBurnRate", u32BurnRateIndex);
        orxConfig_PushSection("Runtime");
        const orxSTRING zName = GetConfigVar("Oil");
        orxFLOAT fLampOil = orxConfig_GetFloat(zName);
        fLampOil = orxMAX(fLampOil - fLampBurnRate * _rstInfo.fDT, orxFLOAT_0);
        orxConfig_SetFloat(zName, fLampOil);
        orxConfig_PopSection();

        // Update light
        orxVECTOR vScale = {orxMAX(fLampOil, orxMATH_KF_EPSILON), orxMAX(fLampOil, orxMATH_KF_EPSILON), orxFLOAT_0};
        GetOwnedChild()->SetScale(vScale);

        // No more oil?
        if(fLampOil == orxFLOAT_0)
        {
            // Game Over
            SetSpeed(orxVECTOR_0);
            SetAnim("Death");
            bIsDead = orxTRUE;
            orxClock_RemoveGlobalTimer(orxNULL, orx2F(-1.0f), this);
        }
        else
        {
            // Select input set
            const orxSTRING zSet = orxInput_GetCurrentSet();
            orxInput_SelectSet(orxConfig_GetString("Input"));

            // Not dashing?
            if(!bIsDashing)
            {
                // Move
                orxVECTOR vSpeed = {orxInput_GetValue("MoveRight") - orxInput_GetValue("MoveLeft"), orxInput_GetValue("MoveDown") - orxInput_GetValue("MoveUp"), orxFLOAT_0};

                // Dash
                if(orxInput_HasBeenActivated("Dash"))
                {
                    if(!orxVector_IsNull(&vSpeed))
                    {
                        orxVector_Mulf(&vSpeed, orxVector_Normalize(&vSpeed, &vSpeed), orxConfig_GetFloat("DashSpeed"));
                        bIsDashing = orxTRUE;
                        orxClock_AddGlobalTimer(ResetDash, orxConfig_GetFloat("DashDuration"), 1, this);
                        ld46::GetInstance().CreateObject("DashSound");
                    }
                }
                else
                {
                    orxVector_Mulf(&vSpeed, &vSpeed, orxConfig_GetFloat("Speed"));
                }

                // Set speed
                SetSpeed(vSpeed);

                // Select anim
                if(vSpeed.fX < orxFLOAT_0)
                {
                    zLastAnim = "RunLeft";
                }
                else if(vSpeed.fX > orxFLOAT_0)
                {
                    zLastAnim = "RunRight";
                }

                // Update Anim
                SetAnim((orxVector_GetSize(&vSpeed) > orxConfig_GetFloat("MoveThreshold")) ? zLastAnim : orxNULL);
            }

            // Deselect input set
            orxInput_SelectSet(zSet);
        }

        PopConfigSection();
    }
}

orxBOOL Player::OnCollide(ScrollObject *_poCollider, const orxSTRING _zPartName, const orxSTRING _zColliderPartName, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal)
{
    if(!bIsDead)
    {
        if(orxString_SearchString(_zColliderPartName, "Oil"))
        {
            orxConfig_PushSection("Runtime");
            orxConfig_SetU64("Collider", GetGUID());
            orxConfig_PopSection();
            _poCollider->AddConditionalTrack("PickUp");
        }
        else if(orxString_SearchString(_zColliderPartName, "Death"))
        {
            orxConfig_PushSection("Runtime");
            orxConfig_SetFloat(GetConfigVar("Oil"), orxFLOAT_0);
            orxConfig_PopSection();
        }
        else
        {
            if (bIsDashing)
            {
                orxConfig_PushSection(_zColliderPartName);
                if (!orxConfig_GetBool("IsDoor"))
                {
                    ld46::GetInstance().CreateObject("HitSound");
                }
                orxConfig_PopSection();
            }
        }
    }

    return orxTRUE;
}
