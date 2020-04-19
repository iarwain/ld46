/**
 * @file Train.cpp
 * @date 18-Apr-2020
 */

#include "Train.h"

void Train::OnCreate()
{
    Object::OnCreate();

    orxConfig_SetBool("IsTrain", orxTRUE);
}

void Train::OnDelete()
{
    Object::OnDelete();
}

void Train::Update(const orxCLOCK_INFO &_rstInfo)
{
    Object::Update(_rstInfo);
}
