#ifndef TYPES_03_57_29_H__
#define TYPES_03_57_29_H__

/*********************************************************************************************************
  created:      2011/10/11
  file base:    Types
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <AuxLib/ObjectCtrl/Management/ObjectManager.h>
#include <AuxLib/Animable/Animable.h>
#include <AuxLib/Seeking/MovableWorld/Avoiders/Avoiders.h>
#include <Components/Environment/EnvView/EnvViewState.h>
#include <Entities/EntityTypeIds.h>
#include <Entities/Base/EntityTypes.h>
#include "CreatureSettings.h"
#include "CreatureShared.h"

namespace game
{
  class EntityEnvCtrlFactory;
  class ICreature;
  class BulletManager;
  class AssistantManager;
  class ShoveBubblesGeomGen;
  
  class CreatureMotionShared : core::AssignmentFree
  {
    public:
      static Ref<CreatureMotionShared> CreateRef(const CRef<SpatialEntityDB>& spatialDB,
        const Ref<aux::MovableWorld>& movableWorld)
      {
        return RefNew<CreatureMotionShared>(spatialDB, movableWorld);
      }
      
      CreatureMotionShared(const CRef<SpatialEntityDB>& spatialDB,
        const Ref<aux::MovableWorld>& movableWorld)
        : m_SpatialDB(spatialDB),
          m_MovableWorld(movableWorld)
      {
      }
        
      const Ref<aux::MovableWorld>& GetMovableWorld() const
      {
        return m_MovableWorld;
      }
      
      const SpatialEntityDB& GetSpatialObjectDB() const
      {
        return *m_SpatialDB;
      }
      
      aux::IObstacleAvoider& GetObstacleAvoider(const aux::SpatialDescList& obstacles);
    private:
      CRef<SpatialEntityDB> m_SpatialDB;
      Ref<aux::MovableWorld> m_MovableWorld;
      aux::SectorialAvoider m_SectorialAvoider;
      aux::VelocityObstacleAvoider m_VelocityObstacleAvoider;
  };
  
  struct CreatureFactoryArgs : public EntityFactoryArgs
  {
    CreatureFactoryArgs()
    {
    }
    
    CreatureFactoryArgs(const EntityFactoryArgs& args)
      : EntityFactoryArgs(args)
    {
    }
    
    const Ref<SpatialEntityDB>& GetSpatialObjectDB() const
    {
      return spatialDB;
    }
    
    Ref<SpatialEntityDB> spatialDB;
    Ref<CreatureMotionShared> motionShared;
    Ref<EntityList::NodeFactory> entityListNodeFactory;
    Ref<EntityEnvCtrlFactory> entityEnvCtrlFactory;
    Ref<BulletManager> bulletManager;
    Ref<AssistantManager> assistantManager;
    CRef<EnvViewState> envViewState;
    Ref<ShoveBubblesGeomGen> shoveBubblesGeomGen;
  };
  
  struct CreatureViewArgs : public EntityViewArgs 
  {
    CreatureViewArgs(const Ref<ResourceFactory>& resFactory, const Ref<aux::Animable>& animable)
      : EntityViewArgs(resFactory),
        animable(animable)
    {
    }
    
    Ref<aux::Animable> animable;
  };

  struct CreatureArgs : public CreatureFactoryArgs
  {
    CreatureArgs()
    {
    }
    
    CreatureArgs(const CreatureFactoryArgs& args)
      : CreatureFactoryArgs(args)
    {
    }
    
    operator CreatureViewArgs() const
    {
      return CreatureViewArgs(resourceFactory, animable);
    }
    
    Ref<aux::PhysAnimable> animable;
    CRef<CreatureShared> shared;
  };
  
  typedef aux::ObjectTypes<ICreature, CreatureSettings, SpatialEntityDB> CreatureTypes;
  typedef aux::ObjectManager<CreatureTypes> ICreatureManager;
  typedef aux::IObjectFactory<CreatureTypes> ICreatureFactory;
  typedef core::Vector<aux::AnimableObjectId, core::TVectorOpt::Pool> BubbleSpawnIds;
  
  template<class TCreature>
  class CreatureFactory : public ICreatureFactory
  {
    public:
      struct AuxArgs
      {
        Ref<aux::AnimableFactory> animableFactory;
        CRef<CreatureShared> creatureShared;
        Float massScale;
      };
      
      static AuxArgs BuildAuxArgs(const CreatureFactoryArgs& args, AnimableId animableId,
        const BubbleSpawnIds& bubbleSpawnIds, Float massScale)
      {
        AuxArgs result;
        result.animableFactory = args.resourceFactory->CreateAnimableFactory(animableId);
        const Ref<CreatureShared>& creatureShared = RefNew<CreatureShared>();
        for(size_t index = 0; index < bubbleSpawnIds.GetSize(); ++index)
        {
          creatureShared->bubbleSpawnInfo.PushBack(
            result.animableFactory->GetObjectInfo(bubbleSpawnIds[index]));
        }
        
        result.creatureShared = creatureShared;
        result.massScale = massScale;
        return result;
      }
      
      static Ref<CreatureFactory> CreateRef(const CreatureFactoryArgs& args, AnimableId animableId,
        const BubbleSpawnIds& bubbleSpawnIds, Float massScale)
      {
        return RefNew<CreatureFactory>(args,
          CreatureFactory::BuildAuxArgs(args, animableId, bubbleSpawnIds, massScale));
      }
      
      CreatureFactory(const CreatureFactoryArgs& args, const AuxArgs& auxArgs)
        : m_Args(args),
          m_AnimableFactory(auxArgs.animableFactory),
          m_CreatureShared(auxArgs.creatureShared),
          m_MassScale(auxArgs.massScale)
      {
      }
      
      virtual TCreature* CreateObject()
      {
        CreatureArgs creatureArgs = m_Args;
        creatureArgs.animable = m_AnimableFactory->CreateAnimable(*m_Args.GetPhysicalWorld(),
          phys::BodyTypeId::DYNAMIC, 1.0f, m_MassScale);
        creatureArgs.shared = m_CreatureShared;
        return new TCreature(creatureArgs);
      }
      
      virtual void DeleteObject(ICreature* creature);
    private:
      const CreatureFactoryArgs m_Args;
      const Ref<aux::AnimableFactory> m_AnimableFactory;
      const CRef<CreatureShared> m_CreatureShared;
      const Float m_MassScale;
  };
}

#endif /* TYPES_03_57_29_H__ */

