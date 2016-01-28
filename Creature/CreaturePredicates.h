#ifndef CREATUREPREDICATES_17_17_45_H__
#define CREATUREPREDICATES_17_17_45_H__

/*********************************************************************************************************
  created:      2011/10/15
  file base:    CreaturePredicates
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Generics/Reference.h>
#include <Math/Overlap/StripSegment2CircleOver.h>
#include <AuxLib/Seeking/Methods/SeekEval.h>
#include <Entities/Base/IEntity.h>

namespace game
{
  struct IgnoreEntityPredicate1
  {
    IgnoreEntityPredicate1(CPtr<aux::IWorldObject> seekObject0)
      : seekObject0(seekObject0)
    {
    }
    
    bool operator()(const CPtr<aux::IWorldObject>& seekObject) const
    {
      return seekObject != seekObject0;
    }
    
    CPtr<aux::IWorldObject> seekObject0;
  };
  
  struct IgnoreEntityCenterPredicate1
  {
    IgnoreEntityCenterPredicate1(CPtr<aux::IWorldObject> seekObject0,
      core::Vector2F center, Float maxDistance)
      : seekObject0(seekObject0),
        center(center),
        maxDistance(maxDistance)
    {
    }
    
    bool operator()(const CPtr<aux::IWorldObject>& seekObject) const
    {
      if(seekObject != seekObject0)
      {
        const Float radius = seekObject->GetRadius();
        return core::Vector2F::Distance(center, seekObject->GetPosition()) - radius < maxDistance;
      }
      
      return false;
    }
    
    CPtr<aux::IWorldObject> seekObject0;
    core::Vector2F center;
    Float maxDistance;
  };

  struct IgnoreEntityPredicate2
  {
    IgnoreEntityPredicate2(CPtr<aux::IWorldObject> seekObject0, CPtr<aux::IWorldObject> seekObject1)
      : seekObject0(seekObject0),
        seekObject1(seekObject1)
    {
    }
    
    bool operator()(const CPtr<aux::IWorldObject>& seekObject) const
    {
      return seekObject != seekObject0 && seekObject != seekObject1;
    }
    
    CPtr<aux::IWorldObject> seekObject0;
    CPtr<aux::IWorldObject> seekObject1;
  };
  
  struct IgnoreEntityCenterPredicate2
  {
    IgnoreEntityCenterPredicate2(CPtr<aux::IWorldObject> seekObject0, CPtr<aux::IWorldObject> seekObject1,
      core::Vector2F center, Float maxDistance)
      : seekObject0(seekObject0),
        seekObject1(seekObject1),
        center(center),
        maxDistance(maxDistance)
    {
    }
    
    bool operator()(const CPtr<aux::IWorldObject>& seekObject) const
    {
      if(seekObject != seekObject0 && seekObject != seekObject1)
      {
        const Float radius = seekObject->GetRadius();
        return core::Vector2F::Distance(center, seekObject->GetPosition()) - radius < maxDistance;
      }
      
      return false;
    }
    
    CPtr<aux::IWorldObject> seekObject0;
    CPtr<aux::IWorldObject> seekObject1;
    core::Vector2F center;
    Float maxDistance;
  };
  
  struct FindObstaclePredicate
  {
    FindObstaclePredicate(core::Vector2F position,
      core::Vector2F linVelocity, Float radius, Float predictionTime)
      : motionStripSegment(position, predictionTime * linVelocity, radius),
        objectPVR(position, linVelocity, radius),
        predictionTime(predictionTime)
    {
    }
    
    bool operator()(const CPtr<IEntity>& entity) const
    {
      const aux::WorldObjectPVR otherObjectPVR(entity->GetPosition(),
        entity->GetTotalLinVelocity(), entity->GetViewRadius());
      if(core::StripSegment2CircleOverF::CheckOverlaps(motionStripSegment,
        core::CircleF(otherObjectPVR.position, otherObjectPVR.radius)))
      {
        const Float approachTime = aux::SeekEval::PredictNearestApproachTime(objectPVR, otherObjectPVR);
        return approachTime < predictionTime;
      }
      
      return false;
    }
    
    core::StripSegmentF motionStripSegment;
    aux::WorldObjectPVR objectPVR;
    Float predictionTime;
  };
}

#endif /* CREATUREPREDICATES_17_17_45_H__ */

