/*********************************************************************************************************
  created:      2013/02/01
  file base:    ICreatureBase
  file ext:     cpp
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include "ICreatureBase.h"

using namespace core;
using namespace aux;

namespace game
{
  ICreatureBase::ICreatureBase()
    : m_SensorModeCounter(0)
  {
  }
  
  void ICreatureBase::Initialize(const CreatureArgs& args,
    const EntityViewList& viewList, const Ref<EntityEnvCtrl>& envCtrl)
  {
    Base::Initialize(args);
    Base::InitializeView(viewList, envCtrl);
  }
  
  void ICreatureBase::IncSensorModeCounter()
  {
    if(++m_SensorModeCounter == 1)
    {
      SetSensorMode(true);
    }
  }
  
  void ICreatureBase::DecSensorModeCounter()
  {
    ENFORCE(m_SensorModeCounter != 0);
    if(--m_SensorModeCounter == 0)
    {
      SetSensorMode(false);
    }
  }
  
  /* virtual */
  void ICreatureBase::SetSettings(const CreatureSettings& settings)
  {
    SetHealth(settings.health);
    SetMaxLinearSpeed(settings.maxLinearSpeed);
    SetMaxLinearAccel(settings.maxLinearAccel);
    SetMaxLinearBraking(settings.maxLinearBraking);
    SetMaxAngularSpeed(settings.maxAngularSpeed);
    SetMaxAngularAccel(settings.maxAngularAccel);
    SetMaxAngularBraking(settings.maxAngularBraking);
  }
}
