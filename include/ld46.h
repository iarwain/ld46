/**
 * @file ld46.h
 * @date 18-Apr-2020
 */

#ifndef __ld46_H__
#define __ld46_H__

#define __NO_SCROLLED__
#include "Scroll/Scroll.h"

/** Game Class
 */
class ld46 : public Scroll<ld46>
{
public:


private:

                orxSTATUS       Bootstrap() const;

                void            Update(const orxCLOCK_INFO &Info);

                orxSTATUS       Init();
                orxSTATUS       Run();
                void            Exit();
                void            BindObjects();


private:
};

#endif // __ld46_H__
