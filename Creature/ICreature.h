#ifndef ICREATURE_03_52_33_H__
#define ICREATURE_03_52_33_H__

/*********************************************************************************************************
  created:      2011/10/11
  file base:    ICreature
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Entities/Effects/Base/IEntityEffect.h>
#include "ICreatureBase.h"
#include "CreatureMotionSys.h"
#include "CreatureStarsView.h"
#include "ICreatureView.h"
#include "CreatureTargets.h"
#include "../ActionBase/IHuntAction.h"
#include "../ActionTypes/SenselessAction.h"
#include "../ActionTypes/PowerlessAction.h"
#include "../ActionTypes/HangAroundAction.h"
#include "../ActionTypes/DyingAction.h"

namespace game
{
  struct CreatureSpatialMultId
  {
    ENUM()
    {
      _0 = SpatialMultId::_0
    };
    STRUCT_ENUM(CreatureSpatialMultId);
  };
  
  struct CreatureActionId
  {
    ENUM()
    {
      NONE,
      IDLE,
      SEEK_POSITION,
      SEEK_ANGLE,
      SEEK_POS_DIR,
      INTERCEPT,
      INTERCEPT_1D,
      PURSUIT,
      ESCAPE,
      SEEK_VELOCITY,
      SEEK_VELOCITY_01,
      BRAKE,
      HUNT
    };
    STRUCT_ENUM(CreatureActionId);
  };
  
  typedef core::SafeDelegate2<ICreature&, IAction::State> ActionStateChangedHandler;
  
  class ICreature : public ICreatureBase,
                    public core::EnableDelegateToThis,
                    protected IActionEnv,
                    protected ISenseControl
  {
    friend class CreatureMotionSys;
    friend class ShoveMode;
    public:
      typedef ICreatureBase Base;
      typedef core::Notifier2<ICreature&, CreatureActionId> ActionChangeNotifier;
      typedef ActionChangeNotifier::NotifiedList ActionChangedNotifiedList;
      
      virtual void AccumViewSet(ViewAccum& viewAccum);
      virtual void SetSettings(const CreatureSettings& settings);
      virtual void HandleTick(TimeStep dt);
      virtual void BeginCollision(IEntity& other);
      virtual aux::OriCircle GetViewCircle() const;
      
      virtual void StartAbility();
      virtual void ApplyEffect(const EntityEffectArgs& effectArgs);
      
      CreatureTypeId GetCreatureType() const;
      void SetPredictionTime(Float predictionTime);
      void SetSpatialMult(Float multiplier);
      Float GetSpatialMult() const;
      void AddSpatialMult(Float addMult);
      void SetSpatialMult(CreatureSpatialMultId multId, Float multiplier, Float duration);
      void CancelSpatialMult(CreatureSpatialMultId multId);
      
      CreatureActionId GetCurrentActionId() const;
      void ResetTarget();
      void GoIdle();
      void SetTargetPosition(const CreatureTargetPos& target, const ActionStateChangedHandler& handler);
      void SetTargetAngle(CreatureTargetAngle target, const ActionStateChangedHandler& handler);
      void SetTargetPosDir(const CreatureTargetPosDir& target, const ActionStateChangedHandler& handler);
      void Intercept(const CreatureTargetIntercept& target, const ActionStateChangedHandler& handler);
      void Intercept1d(const CreatureTargetIntercept1d& target, const ActionStateChangedHandler& handler);
      void Pursuit(const CreatureTargetPursuit& target, const ActionStateChangedHandler& handler);
      void Escape(const CreatureTargetEscape& target);
      void SetTargetVelocity(const CreatureTargetLinVel& target, const ActionStateChangedHandler& handler);
      void SetTargetVelocity01(const CreatureTargetLinVel01& target,
        const ActionStateChangedHandler& handler);
      void Brake(const ActionStateChangedHandler& handler);
      void StartHunting(const CreatureTargetHunt& target, const ActionStateChangedHandler& handler);

      void InterruptPowerless(const CreatureTargetPowerless& target,
        const ActionStateChangedHandler& handler);
      void InterruptHungAround(const CreatureTargetHungAround& target,
        const ActionStateChangedHandler& handler);
      
      void OverstepInterruptionAction();

      void ResetActionStateChangedHandler();
      
      ActionChangedNotifiedList& GetActionChangedNotified();
      
      uint32 GetBonusPointsAmount() const;
      
      bool GetTouchFlag() const;
      void ResetTouchFlag();
      
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A2_NULL(SetTargetPosition);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A2_R_NULL(SetTargetPosVel, Float);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A2_NULL(SetTargetAngle);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A2_NULL(SetTargetPosDir);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A2_NULL(Intercept);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A2_NULL(Intercept1d);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A2_NULL(Pursuit);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A2_NULL(SetTargetVelocity);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A2_NULL(SetTargetVelocity01);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X0_A2_NULL(Brake);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A2_NULL(StartHunting);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A2_NULL(InterruptPowerless);
      SAFE_INJECT_DELEGATE_AUTO_CONSTRUCT_OVERLOAD_T0_X1_A2_NULL(InterruptHungAround);
    protected:
      ICreature(CreatureTypeId creatureTypeId, const CreatureArgs& args, aux::AnimableObjectId starsTexelId);
      void Initialize(const CreatureArgs& args, IMotionObserver* motionObserver,
        const EntityViewList& viewList, IHuntAction& huntAction);
      
      void SetPointMotionType();
      void SetHovercraftMotionType(Float angularFactor);
      void SetSpatialMult2(Float multiplier, Float duration = Float::PlusInfinity());
      void CancelSpatialMult2();
      const Ref<EntityEnvCtrl>& GetEnvCtrl() const;
      const Ref<SpatialEntityDB>& GetSpatialDB() const;
      
      virtual void Create();
      virtual void Activate();
      virtual void Deactivate();
      virtual void KnockOut(Float knockoutTime);
      virtual void HandleDeadlyDamage(const IEntity& source);
      virtual void StartDying(DyingControl& dyingControl);
      
      virtual void HandleStateChange(IActionBase::State currentState);
      
      virtual ICreatureBase& GetCreature();
      virtual const aux::Animable& GetAnimable() const;
      virtual CreatureMotionSys& GetMotionSystem();
      virtual void SetContactHandler(const EntityContactHandler& handler);
      
      virtual Float StartDyingAnimation();
      
      virtual void Faint();
      virtual void ComeTo();
    private:
      typedef core::Variant
      <
        CreatureTargetIdle, CreatureTargetPos, CreatureTargetAngle, CreatureTargetPosDir,
        CreatureTargetIntercept, CreatureTargetIntercept1d, CreatureTargetPursuit, CreatureTargetEscape,
        CreatureTargetLinVel, CreatureTargetLinVel01, CreatureTargetBrake, CreatureTargetHunt
      > TargetArgs;
      struct ActionArgs
      {
        TargetArgs target;
        ActionStateChangedHandler handler;
      };
      typedef core::Variant
      <
        IAction, SenselessAction, PowerlessAction, HangAroundAction, DyingAction
      > InterruptionAction;
      
      void UpdateCurrentActionArgs(const TargetArgs& target, const ActionStateChangedHandler& handler);
      void StartCurrentAction();
      void StopCurrentAction();
      void ResetCurrentAction();
      
      void SetInterruptionAction(IActionBase* interruptionActionPtr);
      void StartInterruptionAction(const InterruptionAction& interruptionAction);
      void ResetInterruptionAction();
      
      void HandleAction(TimeStep dt);
      void HandleCustomAction(TimeStep dt);
      
      void BrakeOnReachState(ICreature& creature, IActionBase::State actionState);
      void SetCurrentActionId(CreatureActionId currentActionId);
      void SetActionStateChangeHandler(const ActionStateChangedHandler& handler);
      void HandleMotionStateChange(aux::MotionTaskState currentState);
      
      void CastEigenToInduced();
      
      const CreatureTypeId m_CreatureTypeId;
      const Ref<aux::MovableWorld> m_MovableWorld;
      const Ref<EntityList::NodeFactory> m_EntityListNodeFactory;
      const Ref<SpatialEntityDB> m_SpatialDB;
      const Ref<aux::PhysAnimable> m_Animable;
      const CRef<CreatureShared> m_Shared;
      
      CreatureStarsView m_StarsView;
      Ptr<ISmoothView> m_View;
      Ref<EntityEnvCtrl> m_EnvCtrl;
      ActionArgs m_CurrentActionArgs;
      CreatureActionId m_CurrentActionId;
      IActionBase::State m_ActionPrevState;
      ActionStateChangedHandler m_StateChangeHandler;
      Float m_DefaultPredictionTime;
      Ptr<CreatureMotionSys::Setup> m_MotionSystemSetup;
      CreatureMotionSys m_MotionSystem;
      ActionChangeNotifier m_ActionChangeNotifier;
      EntityContactHandler m_ContactHandler;
      
      PowerlessAction m_PowerlessAction;
      HangAroundAction m_HangAroundAction;
      InterruptionAction m_InterruptionAction;
      Ptr<IActionBase> m_InterruptionActionPtr;
      Ptr<IActionBase> m_CurrCustomAction;
      Ptr<IHuntAction> m_HuntAction;
      
      uint32 m_BonusPointsAmount;
      
      bool m_TouchFlag;
      
      bool m_SensorModeRequested;
  };

  inline CreatureTypeId ICreature::GetCreatureType() const
  {
    return m_CreatureTypeId;
  }
  
  inline void ICreature::SetPredictionTime(Float predictionTime)
  {
    m_MotionSystemSetup->SetPredictionTime(predictionTime);
  }

  inline ICreature::ActionChangedNotifiedList& ICreature::GetActionChangedNotified()
  {
    return m_ActionChangeNotifier;
  }
  
  inline uint32 ICreature::GetBonusPointsAmount() const
  {
    return m_BonusPointsAmount;
  }
  
  inline bool ICreature::GetTouchFlag() const
  {
    return m_TouchFlag;
  }
  
  inline void ICreature::ResetTouchFlag()
  {
    m_TouchFlag = false;
  }
  
  inline void ICreature::SetPointMotionType()
  {
    m_MotionSystemSetup->SetPointMotionType();
  }
  
  inline void ICreature::SetHovercraftMotionType(Float angularFactor)
  {
    m_MotionSystemSetup->SetHovercraftMotionType(angularFactor);
  }
  
  inline const Ref<EntityEnvCtrl>& ICreature::GetEnvCtrl() const
  {
    return m_EnvCtrl;
  }
  
  inline const Ref<SpatialEntityDB>& ICreature::GetSpatialDB() const
  {
    return m_SpatialDB;
  }
  
  template<class TCreature> /* virtual */
  void CreatureFactory<TCreature>::DeleteObject(ICreature* creature)
  {
    delete creature;
  }
}

#endif /* ICREATURE_03_52_33_H__ */

