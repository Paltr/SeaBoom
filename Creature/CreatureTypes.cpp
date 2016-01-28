/*********************************************************************************************************
  created:      2012/01/27
  file base:    CreatureTypes
  file ext:     cpp
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include "CreatureTypes.h"

using namespace aux;

namespace game
{
  IObstacleAvoider& CreatureMotionShared::GetObstacleAvoider(const SpatialDescList& obstacles)
  {
    if(obstacles.GetSize() > 4)
    {
      return m_SectorialAvoider;
    }
    else
    {
      return m_VelocityObstacleAvoider;
    }
  }
}
