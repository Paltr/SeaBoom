#ifndef CREATUREMOTIONSYS_08_35_08_H__
#define CREATUREMOTIONSYS_08_35_08_H__

/*********************************************************************************************************
  created:      2011/12/16
  file base:    CreatureMotionSys
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Generics/Variant.h>
#include <Generics/SafeDelegate.h>
#include <AuxLib/Seeking/MovableWorld/MotionTypes/HovercraftMotion.h>
#include <AuxLib/Seeking/MovableWorld/MotionTypes/PointMotion.h>
#include <AuxLib/Seeking/MovableWorld/TargetPredicates/SeekPosPred.h>
#include <AuxLib/Seeking/MovableWorld/TargetPredicates/SeekAnglePred.h>
#include <AuxLib/Seeking/MovableWorld/TargetPredicates/SeekPosDirPred.h>
#include <AuxLib/Seeking/MovableWorld/TargetPredicates/Intercept1dObjPred.h>
#include <AuxLib/Seeking/MovableWorld/TargetPredicates/InterceptObjPred.h>
#include <AuxLib/Seeking/MovableWorld/TargetPredicates/PursuitObjPred.h>
#include <AuxLib/Seeking/MovableWorld/TargetPredicates/EscapeObjPred.h>
#include <AuxLib/Seeking/MovableWorld/TargetPredicates/SeekVelocityPred.h>
#include <AuxLib/Seeking/MovableWorld/TargetPredicates/SeekVelocity01Pred.h>
#include <AuxLib/Seeking/MovableWorld/TargetPredicates/BrakingPred.h>
#include <AuxLib/Seeking/MovableWorld/TargetSeeking/InterceptObjSeekingV.h>
#include <AuxLib/Seeking/MovableWorld/TargetSeeking/PursuitObjSeekingV.h>
#include <AuxLib/Seeking/MovableWorld/TargetSeeking/EscapeObjSeekingV.h>
#include <AuxLib/Seeking/MovableWorld/TargetSeeking/HangAroundObjSeekingV.h>
#include "CreatureTypes.h"
#include "IMotionObserver.h"

namespace game
{
  namespace impl
  {
    class ISpatialMotion : public aux::IVelConverter
    {
      public:
        virtual ~ISpatialMotion() {}
        virtual void Update(TimeStep dt, const aux::MovableInfo& movableInfo) = 0;
        virtual aux::MovableVelocities Advance(const aux::TargetDpMvZw& target) const = 0;
        virtual aux::MovableVelocities Advance(const aux::TargetDaZvw& target) const = 0;
        virtual aux::MovableVelocities Advance(const aux::TargetDv& target) const = 0;
        virtual aux::MovableVelocities Advance(const aux::TargetDwZv& target) const = 0;
        virtual aux::MovableVelocities Brake(const aux::TargetZvw& target) const = 0;
    };
    
    typedef aux::HovercraftMotionMixin<ISpatialMotion> CreatureHovercraftMotion;
    typedef aux::PointMotionMixin<ISpatialMotion> CreaturePointMotion;
  }
  
  struct NullTarget
  {
  };
  
  typedef core::Variant
  <
    NullTarget, aux::SeekPosTarget, aux::SeekAngleTarget, aux::SeekPosDirTarget, aux::PursuitObjTarget,
    aux::Intercept1dObjTarget, aux::InterceptObjTarget, aux::EscapeObjTarget, aux::HangAroundObjTarget,
    aux::SeekVelocityTarget, aux::SeekVelocity01Target, aux::BrakingTarget
  > MotionTarget;

  struct MotionTaskType
  {
    ENUM()
    {
      NONE                = MotionTarget::Type<NullTarget>::ID,
      SEEK_POSITION       = MotionTarget::Type<aux::SeekPosTarget>::ID,
      SEEK_ANGLE          = MotionTarget::Type<aux::SeekAngleTarget>::ID,
      SEEK_POS_DIR        = MotionTarget::Type<aux::SeekPosDirTarget>::ID,
      INTERCEPT_OBJECT    = MotionTarget::Type<aux::InterceptObjTarget>::ID,
      INTERCEPT_1D_OBJECT = MotionTarget::Type<aux::Intercept1dObjTarget>::ID,
      PURSUIT_OBJECT      = MotionTarget::Type<aux::PursuitObjTarget>::ID,
      ESCAPE_OBJECT       = MotionTarget::Type<aux::EscapeObjTarget>::ID,
      HANG_AROUND_OBJECT  = MotionTarget::Type<aux::HangAroundObjTarget>::ID,
      SEEK_VELOCITY       = MotionTarget::Type<aux::SeekVelocityTarget>::ID,
      SEEK_VELOCITY_01    = MotionTarget::Type<aux::SeekVelocity01Target>::ID,
      BRAKE               = MotionTarget::Type<aux::BrakingTarget>::ID
    };
    STRUCT_ENUM(MotionTaskType);
  };
  
  class ICreatureBase;
  typedef core::SafeDelegate1<aux::MotionTaskState> MotionSysStateChangeHandler;
  struct SpatialMultId
  {
    ENUM()
    {
      _0,
      _1_RESERVED,
      _2_RESERVED,
      AMOUNT
    };
    STRUCT_ENUM(SpatialMultId);
  };
  
  struct SpatialMult
  {
    static SpatialMult CreateAllMult(Float allMult)
    {
      return SpatialMult(allMult, allMult, allMult, allMult, allMult, allMult);
    }
    
    static SpatialMult CreateLinearMult(Float linearMult)
    {
      return SpatialMult(linearMult, linearMult, linearMult, 1.0f, 1.0f, 1.0f);
    }
    
    static SpatialMult CreateAngularMult(Float angularMult)
    {
      return SpatialMult(1.0f, 1.0f, 1.0f, angularMult, angularMult, angularMult);
    }
    
    static SpatialMult CreateLinAngMult(Float linearMult, Float angularMult)
    {
      return SpatialMult(linearMult, linearMult, linearMult, angularMult, angularMult, angularMult);
    }
    
    SpatialMult()
      : linearSpeedMult(1.0f),
        linearAccelMult(1.0f),
        linearBrakingMult(1.0f),
        angularSpeedMult(1.0f),
        angularAccelMult(1.0f),
        angularBrakingMult(1.0f)
    {
    }
    
    SpatialMult(Float linearSpeedMult, Float linearAccelMult, Float linearBrakingMult,
      Float angularSpeedMult, Float angularAccelMult, Float angularBrakingMult)
      : linearSpeedMult(linearSpeedMult),
        linearAccelMult(linearAccelMult),
        linearBrakingMult(linearBrakingMult),
        angularSpeedMult(angularSpeedMult),
        angularAccelMult(angularAccelMult),
        angularBrakingMult(angularBrakingMult)
    {
    }
    
    Float linearSpeedMult;
    Float linearAccelMult;
    Float linearBrakingMult;
    Float angularSpeedMult;
    Float angularAccelMult;
    Float angularBrakingMult;
  };
  
  class CreatureMotionSys : public aux::IMotionSystem
  {
    public:
      class State
      {
        friend class CreatureMotionSys;
        public:
          State() : motionTarget(NullTarget()) {}
        private:
          MotionTarget motionTarget;
          CRef<aux::IWorldObject> targetObject;
          MotionSysStateChangeHandler stateChangeHandler;
          aux::MotionTaskState prevMotionTaskState;
      };
      
      class Setup
      {
        friend class CreatureMotionSys;
        public:
          Setup();
          void SetMotionObserver(Ptr<IMotionObserver> observer);
          void ResetToDefaults();
          void SetVisiblePlanes(aux::SpatialPlanesMask visiblePlanes);
          void SetPointMotionType();
          void SetHovercraftMotionType(Float angularFactor);
          void SetPredictionTime(Float predictionTime);
          void SetState(const State& state);
          State GetState() const;
        private:
          Setup(CreatureMotionSys* motionSys);
          bool CheckInvalid() const;
          
          CreatureMotionSys* m_MotionSys;
      };
      
      CreatureMotionSys(ICreature& creature, const Ref<CreatureMotionShared>& shared);
      Ptr<Setup> Initialize();
      
      virtual void Activate();
      virtual void Deactivate();
      virtual void UpdateState(TimeStep dt);
      
      void SetSpatialMult(SpatialMultId spatialMultId, Float duration, const SpatialMult& spatialMult);
      const SpatialMult& GetSpatialMult(SpatialMultId spatialMultId) const;
      void CancelSpatialMult(SpatialMultId spatialMultId);
      
      void ResetTarget();
      void SetTargetPosition(const aux::SeekPosTarget& target, const MotionSysStateChangeHandler& handler);
      void SetTargetAngle(const aux::SeekAngleTarget& target, const MotionSysStateChangeHandler& handler);
      void SetTargetPosDir(const aux::SeekPosDirTarget& target, const MotionSysStateChangeHandler& handler);
      void Intercept(const aux::InterceptObjTarget& target, const MotionSysStateChangeHandler& handler);
      void Intercept1d(const aux::InterceptObjTarget& target, const MotionSysStateChangeHandler& handler);
      void Pursuit(const aux::PursuitObjTarget& target, const MotionSysStateChangeHandler& handler);
      void Escape(const aux::EscapeObjTarget& target, const MotionSysStateChangeHandler& handler);
      void HangAround(const aux::HangAroundObjTarget& target, const MotionSysStateChangeHandler& handler);
      void SetTargetVelocity(core::Vector2F velocity, const MotionSysStateChangeHandler& handler);
      void SetTargetVelocity01(core::Vector2F velocity01, const MotionSysStateChangeHandler& handler);
      void Brake(const aux::BrakingTarget& target, const MotionSysStateChangeHandler& handler);
      
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A1_NULL(SetTargetPosition);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A1_NULL(SetTargetAngle);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A1_NULL(SetTargetPosDir);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A1_NULL(Intercept);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A1_NULL(Intercept1d);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A1_NULL(Pursuit);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A1_NULL(Escape);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A1_NULL(HangAround);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A1_NULL(SetTargetVelocity);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A1_NULL(SetTargetVelocity01);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A1_NULL(Brake);
    private:
      struct SpatialMultState
      {
        Float remaining;
        SpatialMult mult;
      };
      
      typedef SpatialMultState SpatialMultStates[SpatialMultId::AMOUNT];
      
      void SetMotionObserver(Ptr<IMotionObserver> observer);
      void ResetToDefaults();
      void SetVisiblePlanes(aux::SpatialPlanesMask visiblePlanes);
      void SetPointMotionType();
      void SetHovercraftMotionType(Float angularFactor);
      void SetPredictionTime(Float predictionTime);
      void SetState(const State& state);
      State GetState() const;
      
      void UpdatePositionSeeking(const aux::SeekPosTarget& target, TimeStep dt);
      void UpdateAngleSeeking(const aux::SeekAngleTarget& target, TimeStep dt);
      void UpdatePosDirSeeking(const aux::SeekPosDirTarget& target, TimeStep dt);
      void UpdateIntercept(const aux::InterceptObjTarget& target, TimeStep dt);
      void UpdateIntercept1d(const aux::Intercept1dObjTarget& target, TimeStep dt);
      void UpdatePursuit(const aux::PursuitObjTarget& target, TimeStep dt);
      void UpdateEscape(const aux::EscapeObjTarget& target, TimeStep dt);
      void UpdateHangAround(const aux::HangAroundObjTarget& target, TimeStep dt);
      void UpdateVelocitySeeking(const aux::SeekVelocityTarget& target, TimeStep dt);
      void UpdateVelocity01Seeking(const aux::SeekVelocity01Target& target, TimeStep dt);
      void UpdateBraking(const aux::BrakingTarget& target, TimeStep dt);
      aux::MotionTaskState CheckSeekPosPredState(const aux::SeekPosTarget& target) const;
      aux::MotionTaskState CheckSeekPosVelPredState(const aux::AdvanceDpvData& context) const;
      aux::MotionTaskState CheckSeekAnglePredState(const aux::SeekAngleTarget& target) const;
      aux::MotionTaskState CheckPursuitPredState(const aux::PursuitObjTarget& target) const;
      void SeekTargetPos(core::Vector2F targetPos, TimeStep dt);
      void SeekTargetAngle(core::AnglePi targetAngle, TimeStep dt);
      void SeekTargetVel01(core::Vector2F velocity01, TimeStep dt);
      void SeekTargetVel(core::Vector2F targetVel, TimeStep dt, Float predictionTime);
      void SeekTargetVel(core::Vector2F targetVel, TimeStep dt);
      void ProceedMotion(TimeStep dt);
      
      void StartTask(const MotionSysStateChangeHandler& handler);
      void UpdateMotionTargetState(aux::MotionTaskState state);
      aux::SpatialDescList GetObstacles(Float predictionTime) const;
      aux::SpatialDescList GetObstacles() const;
      
      void ApplyVelocities(const aux::MovableVelocities& state, TimeStep dt);
      void HandleMotionChanges(const aux::MovableVelocities& prev,
        const aux::MovableVelocities& curr, Float dt);
      
      typedef core::Variant
      <
        impl::ISpatialMotion, impl::CreatureHovercraftMotion, impl::CreaturePointMotion
      > MotionType;
      
      ICreatureBase& m_Creature;
      const Ref<CreatureMotionShared> m_Shared;
      Setup m_Setup;
      
      MotionType m_MotionType;
      aux::SpatialPlanesMask m_VisiblePlanes;
      Float m_PredictionTime;
      
      SpatialMultStates m_SpatialMultStates;
      
      MotionTarget m_MotionTarget;
      CRef<aux::IWorldObject> m_TargetObject;
      MotionSysStateChangeHandler m_StateChangeHandler;
      aux::MotionTaskState m_PrevMotionTaskState;
      
      Ptr<IMotionObserver> m_MotionObserver;
      aux::MovableVelocities m_PreviousVelocities;
  };

  /*************************************************************************************
                                   Inline methods
  **************************************************************************************/
  inline const SpatialMult& CreatureMotionSys::GetSpatialMult(SpatialMultId spatialMultId) const
  {
    const SpatialMultState& state = m_SpatialMultStates[spatialMultId];
    return state.mult;
  }
  
  /*************************************************************************************
                         CreatureMotionSys::Setup inline methods
  **************************************************************************************/
  inline CreatureMotionSys::Setup::Setup()
    : m_MotionSys(NULL)
  {
  }
  
  inline CreatureMotionSys::Setup::Setup(CreatureMotionSys* motionSys)
    : m_MotionSys(motionSys)
  {
  }
  
  inline void CreatureMotionSys::Setup::SetMotionObserver(Ptr<IMotionObserver> observer)
  {
    m_MotionSys->SetMotionObserver(observer);
  }
  
  inline void CreatureMotionSys::Setup::ResetToDefaults()
  {
    m_MotionSys->ResetToDefaults();
  }
  
  inline void CreatureMotionSys::Setup::SetVisiblePlanes(aux::SpatialPlanesMask visiblePlanes)
  {
    m_MotionSys->SetVisiblePlanes(visiblePlanes);
  }
  
  inline void CreatureMotionSys::Setup::SetPointMotionType()
  {
    m_MotionSys->SetPointMotionType();
  }
  
  inline void CreatureMotionSys::Setup::SetHovercraftMotionType(Float angularFactor)
  {
    m_MotionSys->SetHovercraftMotionType(angularFactor);
  }
  
  inline void CreatureMotionSys::Setup::SetPredictionTime(Float predictionTime)
  {
    m_MotionSys->SetPredictionTime(predictionTime);
  }
  
  inline void CreatureMotionSys::Setup::SetState(const State& state)
  {
    m_MotionSys->SetState(state);
  }
  
  inline CreatureMotionSys::State CreatureMotionSys::Setup::GetState() const
  {
    return m_MotionSys->GetState();
  }
  
  inline bool CreatureMotionSys::Setup::CheckInvalid() const
  {
    return m_MotionSys == NULL;
  }
}

#endif /* CREATUREMOTIONSYS_08_35_08_H__ */

