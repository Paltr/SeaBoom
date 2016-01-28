/*********************************************************************************************************
  created:      2011/10/11
  file base:    ICreature
  file ext:     cpp
  author:       Paltr

  purpose:
  history:
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Math/Distance/Segm2PointDist.h>
#include <AuxLib/Seeking/MovableWorld/MovableWorld.h>
#include <Components/Config/GameConfig.h>
#include <Entities/Base/EntityEnvCtrlFactory.h>
#include "ICreature.h"

using namespace core;
using namespace engine;
using namespace aux;

namespace game
{
  ICreature::ICreature(CreatureTypeId creatureTypeId,
    const CreatureArgs& args, AnimableObjectId starsTexelId)
    : m_CreatureTypeId(creatureTypeId),
      m_MovableWorld(args.motionShared->GetMovableWorld()),
      m_EntityListNodeFactory(args.entityListNodeFactory),
      m_SpatialDB(args.spatialDB),
      m_Animable(args.animable),
      m_Shared(args.shared),
      m_StarsView(args.resourceFactory, args.envViewState, args.animable, starsTexelId),
      m_CurrentActionId(CreatureActionId::NONE),
      m_ActionPrevState(IActionBase::State::INVALID),
      m_MotionSystem(*this, args.motionShared),
      m_BonusPointsAmount(0),
      m_SensorModeRequested(false)
  {
    const CreatureCfg& creatureCfg = args.gameConfig->creatures.GetCfg(creatureTypeId);
    m_DefaultPredictionTime = creatureCfg.predictionTime;
    m_MotionSystemSetup = m_MotionSystem.Initialize();
    if(!creatureCfg.hovercraftAngFactor.CheckInvalid())
    {
      SetHovercraftMotionType(creatureCfg.hovercraftAngFactor);
    }
    else
    {
      SetPointMotionType();
    }
    
    m_BonusPointsAmount = creatureCfg.bonusPointsAmount;
  }
  
  void ICreature::Initialize(const CreatureArgs& args, IMotionObserver* motionObserver,
    const EntityViewList& viewList, IHuntAction& huntAction)
  {
    m_EnvCtrl = args.entityEnvCtrlFactory->CreateEnvControl();
    Base::Initialize(args, viewList, m_EnvCtrl);
    SetPhysObject(args.animable);
    SetShapesData(this);
    SetTokensObject(this);
    if(!viewList.smoothViewArray.Empty())
    {
      m_View = viewList.smoothViewArray[0];
    }
    
    if(motionObserver != NULL)
    {
      m_MotionSystemSetup->SetMotionObserver(motionObserver);
    }
    
    IActionEnv* const actionEnv = this;
    m_PowerlessAction.Initialize(actionEnv);
    m_HangAroundAction.Initialize(actionEnv);
    m_HuntAction = &huntAction;
    huntAction.Initialize(actionEnv);
  }
  
  /* virtual */
  void ICreature::AccumViewSet(ViewAccum& viewAccum)
  {
    Base::AccumViewSet(viewAccum);
    if(m_StarsView.CheckActive())
    {
      viewAccum.Add(&m_StarsView);
    }
  }
  
  /* virtual */
  void ICreature::SetSettings(const CreatureSettings& settings)
  {
    Base::SetSettings(settings);
    SetDamageSettings(settings.damageSettings);
  }

  /* virtual */
  void ICreature::HandleTick(TimeStep dt)
  {
    Base::HandleTick(dt);
    HandleAction(dt);
    if(m_StarsView.CheckActive())
    {
      m_StarsView.HandleTick(dt);
    }
    
    if(m_SensorModeRequested)
    {
      SetSensorMode(true);
    }
  }
  
  /* virtual */
  void ICreature::BeginCollision(IEntity& other)
  {
    Base::BeginCollision(other);
    m_TouchFlag = true;
    if(m_ContactHandler)
    {
      m_ContactHandler(other);
    }
  }
  
  /* virtual */
  OriCircle ICreature::GetViewCircle() const
  {
    return m_Animable->GetViewCircle();
  }

  /* virtual */
  void ICreature::StartAbility()
  {
  }
  
  /* virtual */
  void ICreature::ApplyEffect(const EntityEffectArgs& effectArgs)
  {
  }
  
  void ICreature::SetSpatialMult(Float multiplier)
  {
    m_MotionSystem.SetSpatialMult(SpatialMultId::_1_RESERVED, Float::PlusInfinity(),
      SpatialMult::CreateAllMult(multiplier));
  }

  Float ICreature::GetSpatialMult() const
  {
    return m_MotionSystem.GetSpatialMult(SpatialMultId::_1_RESERVED).linearSpeedMult;
  }

  void ICreature::AddSpatialMult(Float addMult)
  {
    const SpatialMult& mult = m_MotionSystem.GetSpatialMult(SpatialMultId::_1_RESERVED);
    SetSpatialMult(mult.linearSpeedMult + addMult);
  }
  
  void ICreature::SetSpatialMult(CreatureSpatialMultId multId, Float multiplier, Float duration)
  {
    m_MotionSystem.SetSpatialMult((SpatialMultId)multId, duration, SpatialMult::CreateAllMult(multiplier));
  }
  
  void ICreature::CancelSpatialMult(CreatureSpatialMultId multId)
  {
    m_MotionSystem.CancelSpatialMult((SpatialMultId)multId);
  }
  
  CreatureActionId ICreature::GetCurrentActionId() const
  {
    return m_CurrentActionId;
  }
  
  void ICreature::ResetTarget()
  {
    SetCurrentActionId(CreatureActionId::NONE);
    m_MotionSystem.ResetTarget();
    SetActionStateChangeHandler(ActionStateChangedHandler());
  }
  
  void ICreature::GoIdle()
  {
    UpdateCurrentActionArgs(CreatureTargetIdle(), ActionStateChangedHandler());
    StartCurrentAction();
  }
  
  void ICreature::SetTargetPosition(const CreatureTargetPos& target, const ActionStateChangedHandler& handler)
  {
    UpdateCurrentActionArgs(target, handler);
    StartCurrentAction();
  }
  
  void ICreature::SetTargetAngle(CreatureTargetAngle target, const ActionStateChangedHandler& handler)
  {
    UpdateCurrentActionArgs(target, handler);
    StartCurrentAction();
  }
  
  void ICreature::SetTargetPosDir(const CreatureTargetPosDir& target, const ActionStateChangedHandler& handler)
  {
    UpdateCurrentActionArgs(target, handler);
    StartCurrentAction();
  }

  void ICreature::Intercept(const CreatureTargetIntercept& target, const ActionStateChangedHandler& handler)
  {
    UpdateCurrentActionArgs(target, handler);
    StartCurrentAction();
  }
  
  void ICreature::Intercept1d(const CreatureTargetIntercept1d& target,
    const ActionStateChangedHandler& handler)
  {
    UpdateCurrentActionArgs(target, handler);
    StartCurrentAction();
  }
  
  void ICreature::Pursuit(const CreatureTargetPursuit& target, const ActionStateChangedHandler& handler)
  {
    UpdateCurrentActionArgs(target, handler);
    StartCurrentAction();
  }
  
  void ICreature::Escape(const CreatureTargetEscape& target)
  {
    UpdateCurrentActionArgs(target, ActionStateChangedHandler());
    StartCurrentAction();
  }
  
  void ICreature::SetTargetVelocity(const CreatureTargetLinVel& target,
    const ActionStateChangedHandler& handler)
  {
    UpdateCurrentActionArgs(target, handler);
    StartCurrentAction();
  }
  
  void ICreature::SetTargetVelocity01(const CreatureTargetLinVel01& target,
    const ActionStateChangedHandler& handler)
  {
    UpdateCurrentActionArgs(target, handler);
    StartCurrentAction();
  }
  
  void ICreature::Brake(const ActionStateChangedHandler& handler)
  {
    UpdateCurrentActionArgs(CreatureTargetBrake(), handler);
    StartCurrentAction();
  }
  
  void ICreature::StartHunting(const CreatureTargetHunt& target, const ActionStateChangedHandler& handler)
  {
    UpdateCurrentActionArgs(target, handler);
    StartCurrentAction();
  }

  void ICreature::InterruptPowerless(const CreatureTargetPowerless& target,
    const ActionStateChangedHandler& handler)
  {
    SetInterruptionAction(&m_PowerlessAction);
    m_PowerlessAction.Setup(target.duration);
    m_PowerlessAction.Start();
  }

  void ICreature::InterruptHungAround(const CreatureTargetHungAround& target,
    const ActionStateChangedHandler& handler)
  {
    SetInterruptionAction(&m_HangAroundAction);
    m_HangAroundAction.Setup(target.object, target.minRadius, target.maxRadius, target.duration);
    m_HangAroundAction.Start();
  }

  void ICreature::OverstepInterruptionAction()
  {
    ResetInterruptionAction();
    StartCurrentAction();
  }

  void ICreature::ResetActionStateChangedHandler()
  {
    m_StateChangeHandler = ActionStateChangedHandler();
  }
  
  void ICreature::SetSpatialMult2(Float multiplier, Float duration /* = Float::PlusInfinity() */)
  {
    m_MotionSystem.SetSpatialMult(SpatialMultId::_2_RESERVED, duration,
      SpatialMult::CreateAllMult(multiplier));
  }
  
  void ICreature::CancelSpatialMult2()
  {
    m_MotionSystem.CancelSpatialMult(SpatialMultId::_2_RESERVED);
  }
  
  /* virtual */
  void ICreature::Create()
  {
    Base::Create();
    m_MotionSystemSetup->SetPredictionTime(m_DefaultPredictionTime);
  }

  /* virtual */
  void ICreature::Activate()
  {
    Base::Activate();
    m_MovableWorld->AddMotionSystem(&m_MotionSystem);
    m_TouchFlag = false;
    if(m_SensorModeRequested)
    {
      SetSensorMode(false);
      m_SensorModeRequested = false;
    }
  }
  
  /* virtual */
  void ICreature::Deactivate()
  {
    ResetActionStateChangedHandler();
    ResetInterruptionAction();
    ResetCurrentAction();
    m_ActionPrevState = IActionBase::State::INVALID;
    m_ActionChangeNotifier.Clear();
    m_ContactHandler = EntityContactHandler();
    m_MovableWorld->RemoveMotionSystem(&m_MotionSystem);
    m_MotionSystem.ResetTarget();
    Base::Deactivate();
  }
  
  /* virtual */
  void ICreature::KnockOut(Float knockoutTime)
  {
    Base::KnockOut(knockoutTime);
    StartInterruptionAction(SenselessAction(*this, knockoutTime));
  }
  
  /* virtual */
  void ICreature::HandleDeadlyDamage(const IEntity& source)
  {
    Base::HandleDeadlyDamage(source);
    GetMissionListener()->HandleCreatureDied(*this, source);
  }
  
  /* virtual */
  void ICreature::StartDying(DyingControl& dyingControl)
  {
    Base::StartDying(dyingControl);
    CastEigenToInduced();
    ResetTarget();
    dyingControl.SetDyingDelay(StartDyingAnimation());
  }
  
  /* virtual */
  void ICreature::HandleStateChange(IActionBase::State currentState)
  {
    if(m_ActionPrevState != currentState)
    {
      if(m_StateChangeHandler)
      {
        m_StateChangeHandler(*this, currentState);
      }

      m_ActionPrevState = currentState;
    }
  }
  
  /* virtual */
  ICreatureBase& ICreature::GetCreature()
  {
    return *this;
  }
  
  /* virtual */
  const Animable& ICreature::GetAnimable() const
  {
    return *m_Animable;
  }
  
  /* virtual */
  CreatureMotionSys& ICreature::GetMotionSystem()
  {
    return m_MotionSystem;
  }
  
  /* virtual */
  void ICreature::SetContactHandler(const EntityContactHandler& handler)
  {
    m_ContactHandler = handler;
  }
  
  /* virtual */
  Float ICreature::StartDyingAnimation()
  {
    Float resultTime;
    
    const Float dyingDuration = 0.5f;
    const size_t bubblesAmount = m_Shared->bubbleSpawnInfo.GetSize();
    if(bubblesAmount != 0)
    {
      StartInterruptionAction(DyingAction(m_Shared, m_EnvCtrl, m_Animable, dyingDuration));
      resultTime = dyingDuration;
    }
    
    if(m_View != NULL)
    {
      m_View->StartDissapear(dyingDuration);
      resultTime = dyingDuration;
    }
    
    m_SensorModeRequested = true;
    return resultTime;
  }
  
  /* virtual */
  void ICreature::Faint()
  {
    CastEigenToInduced();
    m_MotionSystem.ResetTarget();
    if(!m_StarsView.CheckActive())
    {
      m_StarsView.Activate();
    }
  }
  
  /* virtual */
  void ICreature::ComeTo()
  {
    m_StarsView.Deactivate();
  }
  
  inline void ICreature::UpdateCurrentActionArgs(const TargetArgs& target,
    const ActionStateChangedHandler& handler)
  {
    m_CurrentActionArgs.target = target;
    m_CurrentActionArgs.handler = handler;
  }
  
  void ICreature::StartCurrentAction()
  {
    if(m_InterruptionActionPtr == NULL)
    {
      if(m_CurrentActionArgs.target.CheckValid())
      {
        switch(m_CurrentActionArgs.target.GetTypeId())
        {
          case TargetArgs::Type<CreatureTargetIdle>::ID:
          {
            SetCurrentActionId(CreatureActionId::IDLE);
            m_MotionSystem.Brake(GameConstants::GetCreatureIdleLinBreakingFactor(),
              this, &ICreature::HandleMotionStateChange);
            SetActionStateChangeHandler(ActionStateChangedHandler());
            break;
          }
          
          case TargetArgs::Type<CreatureTargetPos>::ID:
          {
            const CreatureTargetPos& target = m_CurrentActionArgs.target;
            SetCurrentActionId(CreatureActionId::SEEK_POSITION);
            m_MotionSystem.SetTargetPosition(target.position, this, &ICreature::HandleMotionStateChange);
            if(m_CurrentActionArgs.handler)
            {
              SetActionStateChangeHandler(m_CurrentActionArgs.handler);
            }
            else
            {
              SetActionStateChangeHandler(ActionStateChangedHandler(this, &ICreature::BrakeOnReachState));
            }
            
            break;
          }
          
          case TargetArgs::Type<CreatureTargetAngle>::ID:
          {
            const CreatureTargetAngle& target = m_CurrentActionArgs.target;
            SetCurrentActionId(CreatureActionId::SEEK_ANGLE);
            m_MotionSystem.SetTargetAngle((AnglePi)target.angle, this, &ICreature::HandleMotionStateChange);
            if(m_CurrentActionArgs.handler)
            {
              SetActionStateChangeHandler(m_CurrentActionArgs.handler);
            }
            else
            {
              SetActionStateChangeHandler(ActionStateChangedHandler(this, &ICreature::BrakeOnReachState));
            }
            
            break;
          }
          
          case TargetArgs::Type<CreatureTargetPosDir>::ID:
          {
            const CreatureTargetPosDir& target = m_CurrentActionArgs.target;
            SetCurrentActionId(CreatureActionId::SEEK_POS_DIR);
            m_MotionSystem.SetTargetPosDir(target.position, this, &ICreature::HandleMotionStateChange);
            if(m_CurrentActionArgs.handler)
            {
              SetActionStateChangeHandler(m_CurrentActionArgs.handler);
            }
            else
            {
              SetActionStateChangeHandler(ActionStateChangedHandler(this, &ICreature::BrakeOnReachState));
            }
            
            break;
          }
          
          case TargetArgs::Type<CreatureTargetIntercept>::ID:
          {
            const CreatureTargetIntercept& target = m_CurrentActionArgs.target;
            SetCurrentActionId(CreatureActionId::INTERCEPT);
            const InterceptObjTarget interceptObjTarget(target.object);
            m_MotionSystem.Intercept(interceptObjTarget, this, &ICreature::HandleMotionStateChange);
            SetActionStateChangeHandler(m_CurrentActionArgs.handler);
            break;
          }
          
          case TargetArgs::Type<CreatureTargetIntercept1d>::ID:
          {
            const CreatureTargetIntercept1d& target = m_CurrentActionArgs.target;
            SetCurrentActionId(CreatureActionId::INTERCEPT_1D);
            const InterceptObjTarget interceptObjTarget(target.object);
            m_MotionSystem.Intercept1d(interceptObjTarget, this, &ICreature::HandleMotionStateChange);
            SetActionStateChangeHandler(m_CurrentActionArgs.handler);
            break;
          }
          
          case TargetArgs::Type<CreatureTargetPursuit>::ID:
          {
            const CreatureTargetPursuit& target = m_CurrentActionArgs.target;
            SetCurrentActionId(CreatureActionId::PURSUIT);
            const PursuitObjTarget pursuitObjTarget(target.creature, target.minDistance, target.maxDistance);
            m_MotionSystem.Pursuit(pursuitObjTarget, this, &ICreature::HandleMotionStateChange);
            SetActionStateChangeHandler(m_CurrentActionArgs.handler);
            break;
          }
          
          case TargetArgs::Type<CreatureTargetEscape>::ID:
          {
            const CreatureTargetEscape& target = m_CurrentActionArgs.target;
            SetCurrentActionId(CreatureActionId::ESCAPE);
            m_MotionSystem.Escape(EscapeObjTarget(target.object), this, &ICreature::HandleMotionStateChange);
            SetActionStateChangeHandler(m_CurrentActionArgs.handler);
            break;
          }
          
          case TargetArgs::Type<CreatureTargetLinVel>::ID:
          {
            const CreatureTargetLinVel& target = m_CurrentActionArgs.target;
            SetCurrentActionId(CreatureActionId::SEEK_VELOCITY);
            m_MotionSystem.SetTargetVelocity(target.linVel, this, &ICreature::HandleMotionStateChange);
            SetActionStateChangeHandler(m_CurrentActionArgs.handler);
            break;
          }
          
          case TargetArgs::Type<CreatureTargetLinVel01>::ID:
          {
            const CreatureTargetLinVel01& target = m_CurrentActionArgs.target;
            SetCurrentActionId(CreatureActionId::SEEK_VELOCITY_01);
            m_MotionSystem.SetTargetVelocity01(target.linVel01, this, &ICreature::HandleMotionStateChange);
            SetActionStateChangeHandler(m_CurrentActionArgs.handler);
            break;
          }
          
          case TargetArgs::Type<CreatureTargetBrake>::ID:
          {
            SetCurrentActionId(CreatureActionId::BRAKE);
            m_MotionSystem.Brake(BrakingTarget(), this, &ICreature::HandleMotionStateChange);
            SetActionStateChangeHandler(m_CurrentActionArgs.handler);
            break;
          }
          
          case TargetArgs::Type<CreatureTargetHunt>::ID:
          {
            const CreatureTargetHunt& target = m_CurrentActionArgs.target;
            SetCurrentActionId(CreatureActionId::HUNT);
            m_HuntAction->ResetState();
            m_HuntAction->Start(IHuntAction::Args(target.target));
            m_CurrCustomAction = m_HuntAction;
            SetActionStateChangeHandler(m_CurrentActionArgs.handler);
            break;
          }
          
          default:
          {
            ENFORCE(false);
          }
        }
      }
    }
  }
  
  void ICreature::StopCurrentAction()
  {
    if(m_CurrCustomAction != NULL)
    {
      m_CurrCustomAction->Stop();
      m_CurrCustomAction = NULL;
    }
  }
  
  void ICreature::ResetCurrentAction()
  {
    StopCurrentAction();
    m_CurrentActionArgs = ActionArgs();
  }
  
  void ICreature::SetInterruptionAction(IActionBase* interruptionActionPtr)
  {
    ResetInterruptionAction();
    ENFORCE(interruptionActionPtr != NULL);
    m_ContactHandler = EntityContactHandler();
    interruptionActionPtr->ResetState();
    m_InterruptionActionPtr = interruptionActionPtr;
  }

  void ICreature::StartInterruptionAction(const InterruptionAction& interruptionAction)
  {
    ENFORCE(interruptionAction.CheckValid());
    if(m_InterruptionActionPtr != NULL)
    {
      OverstepInterruptionAction();
    }
    else
    {
      StopCurrentAction();
    }
    
    m_ContactHandler = EntityContactHandler();
    m_InterruptionAction = interruptionAction;
    const Ptr<IAction> action = m_InterruptionAction.GetBase();
    action->Initialize((IActionEnv*)this);
    action->Start();
    m_InterruptionActionPtr = action;
  }
  
  void ICreature::ResetInterruptionAction()
  {
    if(m_InterruptionActionPtr != NULL)
    {
      m_InterruptionActionPtr->Stop();
      m_InterruptionAction.Reset();
      m_InterruptionActionPtr.Reset();
    }
  }
  
  void ICreature::HandleAction(TimeStep dt)
  {
    if(m_InterruptionActionPtr != NULL)
    {
      m_InterruptionActionPtr->HandleTick(dt);
      const IActionBase::State interrruptedActionState = m_InterruptionActionPtr->GetState();
      switch(interrruptedActionState)
      {
        case IActionBase::State::INVALID:
        case IActionBase::State::CANCELED:
        case IActionBase::State::FINISHED:
        {
          OverstepInterruptionAction();
          break;
        }
        
        default:
        {
          break;
        }
      }
    }
    else
    {
      HandleCustomAction(dt);
    }
  }
  
  void ICreature::HandleCustomAction(TimeStep dt)
  {
    if(m_CurrCustomAction != NULL)
    {
      m_CurrCustomAction->HandleTick(dt);
      const IActionBase::State actionState = (m_CurrCustomAction != NULL ?
        m_CurrCustomAction->GetState() : (IActionBase::State)IActionBase::State::CANCELED);
      HandleStateChange(actionState);
      if(actionState == IActionBase::State::CANCELED || actionState == IActionBase::State::FINISHED)
      {
        if(m_CurrCustomAction != NULL)
        {
          m_CurrCustomAction->Stop();
          m_CurrCustomAction = NULL;
        }
        
        m_CurrentActionId = CreatureActionId::NONE;
      }
    }
    else
    {
      /* motion system's simple action is active */
    }
  }
  
  void ICreature::BrakeOnReachState(ICreature& creature, IActionBase::State actionState)
  {
    if(actionState == IActionBase::State::REACHED)
    {
      Brake();
    }
  }
  
  void ICreature::SetCurrentActionId(CreatureActionId currentActionId)
  {
    StopCurrentAction();
    if(m_CurrentActionId != currentActionId)
    {
      const CreatureActionId prevAction = m_CurrentActionId;
      m_CurrentActionId = currentActionId;
      m_ActionChangeNotifier(*this, prevAction);
    }
  }
  
  void ICreature::SetActionStateChangeHandler(const ActionStateChangedHandler& stateChangeHandler)
  {
    switch(m_ActionPrevState)
    {
      case IActionBase::State::INVALID:
      {
        break;
      }
      
      case IActionBase::State::PROCESSING:
      {
        HandleStateChange(IActionBase::State::CANCELED);
        break;
      }
      
      default:
      {
        HandleStateChange(IActionBase::State::FINISHED);
        break;
      }
    }
    
    m_ActionPrevState = IActionBase::State::PROCESSING;
    m_StateChangeHandler = stateChangeHandler;
  }
  
  void ICreature::HandleMotionStateChange(MotionTaskState currentState)
  {
    if(m_CurrCustomAction == NULL)
    {
      const IActionBase::State motionToCreatureState[MotionTaskState::AMOUNT] =
      {
        IActionBase::State::INVALID, IActionBase::State::PROCESSING, IActionBase::State::REACHED
      };
      const IActionBase::State creatureActionState = motionToCreatureState[currentState];
      HandleStateChange(creatureActionState);
    }
  }
  
  void ICreature::CastEigenToInduced()
  {
    SetLinearVelocity(LinearVelocity(ZERO, GetTotalLinVelocity()));
    SetAngularSpeed(AngularSpeed(ZERO, GetTotalAngSpeed()));
  }
}
