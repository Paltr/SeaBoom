#ifndef CREATURESETTINGS_22_09_37_H__
#define CREATURESETTINGS_22_09_37_H__

/*********************************************************************************************************
  created:      2012/02/01
  file base:    CreatureSettings
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Entities/Base/EntityTypes.h>

namespace game
{
  struct CreatureSettings
  {
    CreatureSettings()
      : playerAllied(false)
    {
    }
    
    Float health;
    DamageSettings damageSettings;
    
    Float maxLinearSpeed;
    Float maxLinearAccel;
    Float maxLinearBraking;
    core::Angle maxAngularSpeed;
    core::Angle maxAngularAccel;
    core::Angle maxAngularBraking;
    bool playerAllied;
  };
}

#endif /* CREATURESETTINGS_22_09_37_H__ */

