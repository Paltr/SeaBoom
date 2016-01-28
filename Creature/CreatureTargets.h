#ifndef ENTITYTARGETS_16_09_05_H__
#define ENTITYTARGETS_16_09_05_H__

/*********************************************************************************************************
  created:      2012/05/22
  file base:    EntityTargets
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Math/Vector2.h>
#include <Generics/Reference.h>

namespace aux { class IWorldObject; }

namespace game
{
  class IEntity;
  class ICreature;
  
  struct CreatureTargetIdle
  {
  };
  
  struct CreatureTargetPos
  {
    CreatureTargetPos() {}
    CreatureTargetPos(core::Vector2F position) : position(position) {}
    
    core::Vector2F position;
  };
  
  struct CreatureTargetAngle
  {
    CreatureTargetAngle() {}
    CreatureTargetAngle(core::Angle angle) : angle(angle) {}
    
    core::Angle angle;
  };
  
  struct CreatureTargetPosDir
  {
    CreatureTargetPosDir() {}
    CreatureTargetPosDir(core::Vector2F position) : position(position) {}
    
    core::Vector2F position;
  };
  
  struct CreatureTargetPosLinVel
  {
    CreatureTargetPosLinVel() {}
    CreatureTargetPosLinVel(core::Vector2F position, core::Vector2F velocity)
      : position(position), velocity(velocity)
    {
    }
    
    core::Vector2F position;
    core::Vector2F velocity;
  };
  
  struct CreatureTargetLinVel
  {
    CreatureTargetLinVel() {}
    CreatureTargetLinVel(core::Vector2F linVel) : linVel(linVel) {}
    
    core::Vector2F linVel;
  };
  
  struct CreatureTargetLinVel01
  {
    CreatureTargetLinVel01() {}
    CreatureTargetLinVel01(core::Vector2F linVel01) : linVel01(linVel01) {}
    
    core::Vector2F linVel01;
  };
  
  struct CreatureTargetIntercept
  {
    CreatureTargetIntercept() {}
    CreatureTargetIntercept(const CRef<aux::IWorldObject>& object) : object(object) {}
    
    CRef<aux::IWorldObject> object;
  };
  
  struct CreatureTargetIntercept1d
  {
    CreatureTargetIntercept1d() {}
    CreatureTargetIntercept1d(const CRef<aux::IWorldObject>& object) : object(object) {}
    
    CRef<aux::IWorldObject> object;
  };
  
  struct CreatureTargetPursuit
  {
    CreatureTargetPursuit() {}
    CreatureTargetPursuit(const Ref<ICreature>& creature, Float minDistance, Float maxDistance)
      : creature(creature),
        minDistance(minDistance),
        maxDistance(maxDistance)
    {
    }
    
    Ref<ICreature> creature;
    Float minDistance;
    Float maxDistance;
  };
  
  struct CreatureTargetEscape
  {
    CreatureTargetEscape() {}
    CreatureTargetEscape(const CRef<aux::IWorldObject>& object, Float maxLinSpeed = Float::Invalid())
      : object(object)
    {
    }
    
    CRef<aux::IWorldObject> object;
  };

  struct CreatureTargetHungAround
  {
    CreatureTargetHungAround() : duration(Float::PlusInfinity()) {}
    CreatureTargetHungAround(const CRef<aux::IWorldObject>& object,
      Float minRadius, Float maxRadius, Float duration = Float::PlusInfinity())
      : object(object),
        minRadius(minRadius),
        maxRadius(maxRadius),
        duration(duration)
    {
    }

    CRef<aux::IWorldObject> object;
    Float minRadius;
    Float maxRadius;
    Float duration;
  };
  
  struct CreatureTargetBrake
  {
  };
  
  struct CreatureTargetHunt
  {
    CreatureTargetHunt() {}
    CreatureTargetHunt(const Ref<IEntity>& target)
      : target(target)
    {
    }
    
    Ref<IEntity> target;
  };

  struct CreatureTargetPowerless
  {
    CreatureTargetPowerless(Float duration = Float::PlusInfinity())
      : duration(duration)
    {
    }

    Float duration;
  };
}

#endif /* ENTITYTARGETS_16_09_05_H__ */

