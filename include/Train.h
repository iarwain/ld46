/**
 * @file Train.h
 * @date 18-Apr-2020
 */

#ifndef __TRAIN_H__
#define __TRAIN_H__

#include "Object.h"

/** Train Class
 */
class Train : public Object
{
public:

                orxU64          u64PreviousGUID;


protected:

                void            OnCreate();
                void            OnDelete();
                void            Update(const orxCLOCK_INFO &Info);


private:
};

#endif // __TRAIN_H__
