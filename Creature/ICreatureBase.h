#ifndef ICREATUREBASE_08_20_23_H__
#define ICREATUREBASE_08_20_23_H__

/*********************************************************************************************************
  created:      2012/07/17
  file base:    ICreatureBase
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <AuxLib/Seeking/Interfaces/IManagedSeekObject.h>
#include <AuxLib/Seeking/Mixines/SeekObject/SpatialDBObjectMixin.h>
#include <AuxLib/Seeking/Mixines/SeekObject/PhysObjectMixin.h>
#include <AuxLib/Seeking/Mixines/Helpers.h>
#include <Entities/Base/ITypedEntity.h>
#include <Entities/Helpers/PhysEntityMixin.h>
#include "CreatureTypes.h"

namespace game
{
  namespace impl
  {
    typedef ITypedEntity<IEntity::TypeId::CREATURE, AllianceId::ENEMY> CreatureEntity;
    typedef aux::IManagedSeekObjectMixin<CreatureEntity, CreatureTypes> ICreatureBase0;
    APPLY_SEEK_MIXINES_2(ICreatureBase0, CreatureTypes, aux::SpatialDBObjectMixin,
      PhysEntityMixin, ICreatureBase1);
  }
  
  class ICreatureBase : public impl::ICreatureBase1
  {
    public:
      typedef impl::ICreatureBase1 Base;
      
      ICreatureBase();
      
      void Initialize(const CreatureArgs& args, const EntityViewList& viewList,
        const Ref<EntityEnvCtrl>& envCtrl);
      void IncSensorModeCounter();
      void DecSensorModeCounter();
      
      virtual void SetSettings(const CreatureSettings& settings);
    private:
      size_t m_SensorModeCounter;
  };
}

#endif /* ICREATUREBASE_08_20_23_H__ */

