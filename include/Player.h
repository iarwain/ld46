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


private:
                orxBOOL         IsDashing;
    static      void orxFASTCALL ResetDash(const orxCLOCK_INFO* _pstClockInfo, void* _pContext);
};

#endif // __PLAYER_H__
