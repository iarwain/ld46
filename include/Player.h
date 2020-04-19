/**
 * @file Player.h
 * @date 18-Apr-2020
 */

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Object.h"

/** Object Class
 */
class Player : public Object
{
public:

                orxBOOL         IsDead() const  {return bIsDead;}


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &Info);
                orxBOOL         OnCollide(ScrollObject *_poCollider, const orxSTRING _zPartName, const orxSTRING _zColliderPartName, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal);


private:
    static      void orxFASTCALL ResetDash(const orxCLOCK_INFO* _pstClockInfo, void* _pContext);
    static      void orxFASTCALL UpdateBurnRate(const orxCLOCK_INFO* _pstClockInfo, void* _pContext);

    const       orxSTRING       GetConfigVar(const orxSTRING _zVar) const;

    const       orxSTRING       zLastAnim;
                orxU32          u32BurnRateIndex;
                orxBOOL         bIsDashing;
                orxBOOL         bIsDead;
};

#endif // __PLAYER_H__
