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


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &Info);
                orxBOOL         OnCollide(ScrollObject *_poCollider, const orxSTRING _zPartName, const orxSTRING _zColliderPartName, const orxVECTOR &_rvPosition, const orxVECTOR &_rvNormal);


private:
                orxBOOL         bIsDashing;
    static      void orxFASTCALL ResetDash(const orxCLOCK_INFO* _pstClockInfo, void* _pContext);
};

#endif // __PLAYER_H__
