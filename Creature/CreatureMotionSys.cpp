/*********************************************************************************************************
  created:      2011/12/16
  file base:    CreatureMotion
  file ext:     cpp
  author:       Paltr

  purpose:
  history:
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include "CreatureMotionSys.h"
#include "ICreature.h"
#include "CreaturePredicates.h"

using namespace core;
using namespace aux;
using namespace game::impl;

namespace game
{
  CreatureMotionSys::CreatureMotionSys(ICreature& creature, const Ref<CreatureMotionShared>& shared)
    : m_Creature(creature),
      m_Shared(shared),
      m_VisiblePlanes(SpatialPlaneFlag::ALL),
      m_PrevMotionTaskState(MotionTaskState::INVALID)
  {
  }
  
  Ptr<CreatureMotionSys::Setup> CreatureMotionSys::Initialize()
  {
    ENFORCE(m_Setup.CheckInvalid());
    m_Setup = Setup(this);
    return &m_Setup;
  }

  /* virtual */
  void CreatureMotionSys::Activate()
  {
    ResetToDefaults();
  }
  
  /* virtual */
  void CreatureMotionSys::Deactivate()
  {
    for(size_t index = 0; index < elements_in(m_SpatialMultStates); ++index)
    {
      CancelSpatialMult((SpatialMultId)index);
    }
  }
  
  /* virtual */
  void CreatureMotionSys::UpdateState(TimeStep dt)
  {
    for(size_t index = 0; index < elements_in(m_SpatialMultStates); ++index)
    {
      SpatialMultState& state = m_SpatialMultStates[index];
      if(state.remaining != 0.0f)
      {
        state.remaining -= dt;
        if(state.remaining.Max(0.0f) == 0.0f)
        {
          CancelSpatialMult((SpatialMultId)index);
        }
      }
    }
    
    if(ISpatialMotion* const motion = m_MotionType.GetBase())
    {
      const MovableInfo& movableInfo = m_Creature.GetMovableInfo();
      m_PreviousVelocities = movableInfo.GetVelocities();
      motion->Update(dt, movableInfo);
    }
    
    switch(m_MotionTarget.GetTypeId())
    {
      case MotionTaskType::NONE:
      {
        break;
      }
      
      case MotionTaskType::SEEK_POSITION:
      {
        UpdatePositionSeeking(m_MotionTarget, dt);
        break;
      }
      
      case MotionTaskType::SEEK_ANGLE:
      {
        UpdateAngleSeeking(m_MotionTarget, dt);
        break;
      }
      
      case MotionTaskType::SEEK_POS_DIR:
      {
        UpdatePosDirSeeking(m_MotionTarget, dt);
        break;
      }
      
      case MotionTaskType::INTERCEPT_OBJECT:
      {
        UpdateIntercept(m_MotionTarget, dt);
        break;
      }
      
      case MotionTaskType::INTERCEPT_1D_OBJECT:
      {
        UpdateIntercept1d(m_MotionTarget, dt);
        break;
      }
      
      case MotionTaskType::PURSUIT_OBJECT:
      {
        UpdatePursuit(m_MotionTarget, dt);
        break;
      }

      case MotionTaskType::ESCAPE_OBJECT:
      {
        UpdateEscape(m_MotionTarget, dt);
        break;
      }
      
      case MotionTaskType::HANG_AROUND_OBJECT:
      {
        UpdateHangAround(m_MotionTarget, dt);
        break;
      }
      
      case MotionTaskType::SEEK_VELOCITY:
      {
        UpdateVelocitySeeking(m_MotionTarget, dt);
        break;
      }

      case MotionTaskType::SEEK_VELOCITY_01:
      {
        UpdateVelocity01Seeking(m_MotionTarget, dt);
        break;
      }

      case MotionTaskType::BRAKE:
      {
        UpdateBraking(m_MotionTarget, dt);
        break;
      }

      default:
      {
        ENFORCE(false)("Unknown target type");
        break;
      }
    }
  }
  
  void CreatureMotionSys::SetSpatialMult(SpatialMultId spatialMultId,
    Float duration, const SpatialMult& spatialMult)
  {
    ENFORCE(spatialMult.linearSpeedMult >= 0.0f);
    ENFORCE(spatialMult.linearAccelMult >= 0.0f);
    ENFORCE(spatialMult.linearBrakingMult >= 0.0f);
    ENFORCE(spatialMult.angularSpeedMult >= 0.0f);
    ENFORCE(spatialMult.angularAccelMult >= 0.0f);
    ENFORCE(spatialMult.angularBrakingMult >= 0.0f);
    CancelSpatialMult(spatialMultId);
    SpatialMultState& state = m_SpatialMultStates[spatialMultId];
    state.remaining = duration;
    state.mult = spatialMult;
    m_Creature.SetMaxLinearSpeed(state.mult.linearSpeedMult * m_Creature.GetMaxLinearSpeed());
    m_Creature.SetMaxLinearAccel(state.mult.linearAccelMult * m_Creature.GetMaxLinearAccel());
    m_Creature.SetMaxLinearBraking(state.mult.linearBrakingMult * m_Creature.GetMaxLinearBraking());
    m_Creature.SetMaxAngularSpeed(state.mult.angularSpeedMult * m_Creature.GetMaxAngularSpeed());
    m_Creature.SetMaxAngularAccel(state.mult.angularAccelMult * m_Creature.GetMaxAngularAccel());
    m_Creature.SetMaxAngularBraking(state.mult.angularBrakingMult * m_Creature.GetMaxAngularBraking());
  }
  
  void CreatureMotionSys::CancelSpatialMult(SpatialMultId spatialMultId)
  {
    SpatialMultState& state = m_SpatialMultStates[spatialMultId];
    if(state.mult.linearSpeedMult != 1.0f)
    {
      m_Creature.SetMaxLinearSpeed(m_Creature.GetMaxLinearSpeed() / state.mult.linearSpeedMult);
      m_Creature.SetMaxLinearAccel(m_Creature.GetMaxLinearAccel() / state.mult.linearAccelMult);
      m_Creature.SetMaxLinearBraking(m_Creature.GetMaxLinearBraking() / state.mult.linearBrakingMult);
      m_Creature.SetMaxAngularSpeed(m_Creature.GetMaxAngularSpeed() / state.mult.angularSpeedMult);
      m_Creature.SetMaxAngularAccel(m_Creature.GetMaxAngularAccel() / state.mult.angularAccelMult);
      m_Creature.SetMaxAngularBraking(m_Creature.GetMaxAngularBraking() / state.mult.angularBrakingMult);
      
      state.remaining = 0.0f;
      state.mult = SpatialMult();
    }
  }
  
  void CreatureMotionSys::ResetTarget()
  {
    StartTask(MotionSysStateChangeHandler());
    m_MotionTarget = NullTarget();
  }
  
  void CreatureMotionSys::SetTargetPosition(const SeekPosTarget& target,
    const MotionSysStateChangeHandler& handler)
  {
    StartTask(handler);
    m_MotionTarget = target;
  }
  
  void CreatureMotionSys::SetTargetAngle(const SeekAngleTarget& target,
    const MotionSysStateChangeHandler& handler)
  {
    StartTask(handler);
    m_MotionTarget = target;
  }
  
  void CreatureMotionSys::SetTargetPosDir(const SeekPosDirTarget& target,
    const MotionSysStateChangeHandler& handler)
  {
    StartTask(handler);
    m_MotionTarget = target;
  }
  
  void CreatureMotionSys::Intercept(const InterceptObjTarget& target,
    const MotionSysStateChangeHandler& handler)
  {
    StartTask(handler);
    m_MotionTarget = target;
    m_TargetObject = target.target;
  }
  
  void CreatureMotionSys::Intercept1d(const InterceptObjTarget& target,
    const MotionSysStateChangeHandler& handler)
  {
    const Vector2F targetOffset = target.target->GetPosition() - m_Creature.GetPosition();
    const Intercept1dObjTarget target1d(target, RotF(targetOffset));
    StartTask(handler);
    m_MotionTarget = target1d;
    m_TargetObject = target1d.target;
  }
  
  void CreatureMotionSys::Pursuit(const PursuitObjTarget& target,
    const MotionSysStateChangeHandler& handler)
  {
    StartTask(handler);
    m_MotionTarget = target;
    m_TargetObject = target.target;
  }
  
  void CreatureMotionSys::Escape(const EscapeObjTarget& target,
    const MotionSysStateChangeHandler& handler)
  {
    StartTask(handler);
    m_MotionTarget = target;
  }
  
  void CreatureMotionSys::HangAround(const HangAroundObjTarget& target,
    const MotionSysStateChangeHandler& handler)
  {
    StartTask(handler);
    m_MotionTarget = target;
  }
  
  void CreatureMotionSys::SetTargetVelocity(Vector2F velocity, const MotionSysStateChangeHandler& handler)
  {
    StartTask(handler);
    m_MotionTarget = SeekVelocityTarget(velocity);
  }
  
  void CreatureMotionSys::SetTargetVelocity01(Vector2F velocity01,
    const MotionSysStateChangeHandler& handler)
  {
    StartTask(handler);
    m_MotionTarget = SeekVelocity01Target(velocity01);
  }
  
  void CreatureMotionSys::Brake(const BrakingTarget& target, const MotionSysStateChangeHandler& handler)
  {
    StartTask(handler);
    m_MotionTarget = target;
  }
  
  void CreatureMotionSys::SetMotionObserver(Ptr<IMotionObserver> observer)
  {
    m_MotionObserver = observer;
  }
  
  void CreatureMotionSys::ResetToDefaults()
  {
    ResetTarget();
    m_PrevMotionTaskState = MotionTaskState::INVALID;
  }
  
  void CreatureMotionSys::SetVisiblePlanes(SpatialPlanesMask visiblePlanes)
  {
    m_VisiblePlanes = visiblePlanes;
  }
  
  void CreatureMotionSys::SetPointMotionType()
  {
    CreaturePointMotion motion;
    m_MotionType = motion;
  }
  
  void CreatureMotionSys::SetHovercraftMotionType(Float angularFactor)
  {
    CreatureHovercraftMotion motion;
    motion.SetAngularFactor(angularFactor);
    m_MotionType = motion;
  }
  
  void CreatureMotionSys::SetPredictionTime(Float predictionTime)
  {
    m_PredictionTime = predictionTime;
  }
  
  void CreatureMotionSys::SetState(const State& state)
  {
    m_MotionTarget = state.motionTarget;
    m_TargetObject = state.targetObject;
    m_StateChangeHandler = state.stateChangeHandler;
    m_PrevMotionTaskState = state.prevMotionTaskState;
  }
  
  CreatureMotionSys::State CreatureMotionSys::GetState() const
  {
    State result;
    result.motionTarget = m_MotionTarget;
    result.targetObject = m_TargetObject;
    result.stateChangeHandler = m_StateChangeHandler;
    result.prevMotionTaskState = m_PrevMotionTaskState;
    return result;
  }
  
  void CreatureMotionSys::UpdatePositionSeeking(const SeekPosTarget& target, TimeStep dt)
  {
    const MotionTaskState targetState = CheckSeekPosPredState(target);
    if(targetState == MotionTaskState::SEEKING)
    {
      SeekTargetPos(target.position, dt);
    }
    else
    {
      ProceedMotion(dt);
    }

    UpdateMotionTargetState(targetState);
  }
  
  void CreatureMotionSys::UpdateAngleSeeking(const SeekAngleTarget& target, TimeStep dt)
  {
    const MotionTaskState targetState = CheckSeekAnglePredState(target);
    if(targetState == MotionTaskState::SEEKING)
    {
      SeekTargetAngle(target.angle, dt);
    }
    else
    {
      ProceedMotion(dt);
    }

    UpdateMotionTargetState(targetState);
  }
  
  void CreatureMotionSys::UpdatePosDirSeeking(const SeekPosDirTarget& target, TimeStep dt)
  {
    UpdateAngleSeeking(target.GetSeekAngleTarget(m_Creature), dt);
  }
  
  void CreatureMotionSys::UpdateIntercept(const InterceptObjTarget& target, TimeStep dt)
  {
    const MotionTaskState targetState = InterceptObjPredMethod::Check(m_Creature,
      target, InterceptObjPredSettings(dt));
    if(targetState == MotionTaskState::SEEKING)
    {
      InterceptObjSeekingVRes interceptResult;
      InterceptObjMethodV::Seek(m_Creature, target, m_PredictionTime, interceptResult);
      SeekTargetVel(interceptResult.velocity, dt);
    }
    else
    {
      ProceedMotion(dt);
    }
    
    UpdateMotionTargetState(targetState);
  }
  
  void CreatureMotionSys::UpdateIntercept1d(const Intercept1dObjTarget& target, TimeStep dt)
  {
    const MotionTaskState targetState = Intercept1dObjPredMethod::Check(m_Creature,
      target, Intercept1dObjPredSettings(dt));
    if(targetState == MotionTaskState::SEEKING)
    {
      InterceptObjSeekingVRes interceptResult;
      InterceptObjMethodV::Seek(m_Creature, target, m_PredictionTime, interceptResult);
      SeekTargetVel(interceptResult.velocity, dt);
    }
    else
    {
      ProceedMotion(dt);
    }
    
    UpdateMotionTargetState(targetState);
  }
  
  void CreatureMotionSys::UpdatePursuit(const PursuitObjTarget& target, TimeStep dt)
  {
    const MotionTaskState targetState = CheckPursuitPredState(target);
    if(targetState == MotionTaskState::SEEKING)
    {
      PursuitObjSeekingVRes pursuitResult;
      PursuitObjMethodV::Seek(m_Creature, target, PursuitObjSettingsV(), pursuitResult);
      SeekTargetVel(pursuitResult.velocity, dt);
    }
    else
    {
      const Vector2F currentPosition = m_Creature.GetPosition();
      const Vector2F targetPosition = target.target->GetPosition();
      const AnglePi targetAngle = (targetPosition - currentPosition).GetAngle();
      const Float targetSpeed = target.target->GetTotalLinVelocity().Length();
      if(targetSpeed == 0.0f)
      {
        const Vector2F targetLinVelocity = Vector2F(targetSpeed, 0.0f).Rotate(targetAngle);
        SeekTargetVel(targetLinVelocity, dt);
      }
      else
      {
        UpdateAngleSeeking(SeekAngleTarget(targetAngle), dt);
      }
    }

    UpdateMotionTargetState(targetState);
  }

  void CreatureMotionSys::UpdateEscape(const EscapeObjTarget& target, TimeStep dt)
  {
    EscapeObjSeekingVRes escapeResult;
    EscapeObjMethod::Seek(m_Creature, target, m_PredictionTime, escapeResult);
    SeekTargetVel(escapeResult.velocity, dt);
  }
  
  void CreatureMotionSys::UpdateHangAround(const HangAroundObjTarget& target, TimeStep dt)
  {
    HangAroundObjSeekingVRes hangAroundResult;
    HangAroundObjSeekingV seeker(target.minRadius, target.maxRadius);
    seeker.Seek(m_Creature, target, hangAroundResult);
    SeekTargetVel(hangAroundResult.velocity, dt);
  }
  
  void CreatureMotionSys::UpdateVelocitySeeking(const SeekVelocityTarget& target, TimeStep dt)
  {
    const Float maxDeviation = 1e-3f;
    const SeekVelocityTarget seekTarget(target.velocity);
    const MotionTaskState targetState = SeekVelocityPredMethod::Check(m_Creature, seekTarget, maxDeviation);
    SeekTargetVel(target.velocity, dt);
    UpdateMotionTargetState(targetState);
  }

  void CreatureMotionSys::UpdateVelocity01Seeking(const SeekVelocity01Target& target, TimeStep dt)
  {
    const Float maxDeviation = 1e-3f * target.velocity01.LengthSqr();
    const SeekVelocity01Target seekTarget(target.velocity01);
    const MotionTaskState targetState =
      SeekVelocity01PredMethod::Check(m_Creature, seekTarget, maxDeviation);
    SeekTargetVel01(target.velocity01, dt);
    UpdateMotionTargetState(targetState);
  }

  void CreatureMotionSys::UpdateBraking(const BrakingTarget& target, TimeStep dt)
  {
    const Float maxDeviation = 1e-3f;
    const MotionTaskState targetState = BrakingPredMethod::Check(m_Creature, maxDeviation);
    if(targetState == MotionTaskState::SEEKING)
    {
      if(ISpatialMotion* const motion = m_MotionType.GetBase())
      {
        const MovableVelocities& velocities = motion->Brake(
          TargetZvw(target.brakingLinFactor, target.brakingAngFactor));
        ApplyVelocities(velocities, dt);
      }
    }

    UpdateMotionTargetState(targetState);
  }

  MotionTaskState CreatureMotionSys::CheckSeekPosPredState(const SeekPosTarget& target) const
  {
    const Float distanceSqr = Math::DistanceSqr(target.position, m_Creature.GetPosition());
    const Float radius = m_Creature.GetRadius();
    const Float threshold = radius / 4.0f;
    return (distanceSqr > threshold * threshold ?
      MotionTaskState::SEEKING : MotionTaskState::REACHED);
  }
  
  MotionTaskState CreatureMotionSys::CheckSeekPosVelPredState(const AdvanceDpvData& context) const
  {
    if(CheckSeekPosPredState(SeekPosTarget(context.endState.position)) == MotionTaskState::REACHED)
    {
      const Vector2F currVelocity01 = m_Creature.GetTotalLinVelocity() / m_Creature.GetMaxLinearSpeed();
      if(Math::DistanceSqr(currVelocity01, context.endState.linVelocity) < 1e-2f)
      {
        return MotionTaskState::REACHED;
      }
    }
    
    return MotionTaskState::SEEKING;
  }

  MotionTaskState CreatureMotionSys::CheckSeekAnglePredState(const SeekAngleTarget& target) const
  {
    const Angle maxAngDeviation = 1e-2f;
    return SeekAnglePredMethod::Check(m_Creature, target, maxAngDeviation);
  }
  
  MotionTaskState CreatureMotionSys::CheckPursuitPredState(const PursuitObjTarget& target) const
  {
    const Float distanceSqr = SeekEval::GetExactDistanceSqr(m_Creature, *target.target);
    if(distanceSqr > Square(target.minDistance) && distanceSqr < Square(target.maxDistance))
    {
      return MotionTaskState::REACHED;
    }
    
    return MotionTaskState::SEEKING;
  }

  void CreatureMotionSys::SeekTargetPos(Vector2F targetPos, TimeStep dt)
  {
    if(ISpatialMotion* const motion = m_MotionType.GetBase())
    {
      MovableVelocities velocities = motion->Advance(TargetDpMvZw(targetPos));
      Vector2F targetVelocity = velocities.GetTotalLinVelocity();
      if(targetVelocity != ZERO)
      {
        const SpatialDescList obstacles = GetObstacles();
        if(!obstacles.Empty())
        {
          const MovableInfo& movableInfo = m_Creature.GetMovableInfo();
          IObstacleAvoider& avoider = m_Shared->GetObstacleAvoider(obstacles);
          if(avoider.CorrectVelocity(dt, movableInfo, obstacles, *motion, targetVelocity))
          {
            velocities = motion->Advance(TargetDv(targetVelocity));
          }
        }
      }

      ApplyVelocities(velocities, dt);
    }
  }
  
  void CreatureMotionSys::SeekTargetAngle(AnglePi targetAngle, TimeStep dt)
  {
    if(ISpatialMotion* const motion = m_MotionType.GetBase())
    {
      const MovableVelocities velocities = motion->Advance(TargetDaZvw(targetAngle));
      ApplyVelocities(velocities, dt);
    }
  }
  
  void CreatureMotionSys::SeekTargetVel01(Vector2F velocity01, TimeStep dt)
  {
    SeekTargetVel(m_Creature.GetMaxLinearSpeed() * velocity01, dt);
  }
  
  void CreatureMotionSys::SeekTargetVel(Vector2F targetVel, TimeStep dt, Float predictionTime)
  {
    if(ISpatialMotion* const motion = m_MotionType.GetBase())
    {
      MovableVelocities velocities;
      const SpatialDescList obstacles = GetObstacles(predictionTime);
      if(!obstacles.Empty())
      {
        const MovableInfo& movableInfo = m_Creature.GetMovableInfo();
        IObstacleAvoider& avoider = m_Shared->GetObstacleAvoider(obstacles);
        Vector2F finalVelocity = targetVel;
        if(avoider.CorrectVelocity(dt, movableInfo, obstacles, *motion, finalVelocity))
        {
          velocities = motion->Advance(TargetDv(finalVelocity));
        }
        else
        {
          velocities = motion->Advance(TargetDv(targetVel));
        }
      }
      else
      {
        velocities = motion->Advance(TargetDv(targetVel));
      }
      
      ApplyVelocities(velocities, dt);
    }
  }
  
  inline void CreatureMotionSys::SeekTargetVel(Vector2F targetVel, TimeStep dt)
  {
    SeekTargetVel(targetVel, dt, m_PredictionTime);
  }
  
  void CreatureMotionSys::ProceedMotion(TimeStep dt)
  {
    SeekTargetVel(m_Creature.GetTotalLinVelocity(), dt);
  }

  void CreatureMotionSys::StartTask(const MotionSysStateChangeHandler& handler)
  {
    m_TargetObject.Reset();
    m_PrevMotionTaskState = MotionTaskState::SEEKING;
    m_StateChangeHandler = handler;
  }

  void CreatureMotionSys::UpdateMotionTargetState(MotionTaskState state)
  {
    if(m_PrevMotionTaskState != state)
    {
      if(m_StateChangeHandler)
      {
        m_StateChangeHandler(state);
      }
      
      m_PrevMotionTaskState = state;
    }
  }
  
  SpatialDescList CreatureMotionSys::GetObstacles(Float predictionTime) const
  {
    if(predictionTime != ZERO)
    {
      const SpatialEntityDB& spatialDB = m_Shared->GetSpatialObjectDB();
      const Vector2F position = m_Creature.GetPosition();
      const Float collectRadius = GameConstants::GetSpatialDBSize() / 4.0f;
      const Float radius = m_Creature.GetRadius();
      const Float visibilityRadius = radius + predictionTime * m_Creature.GetMaxLinearSpeed();
      if(m_TargetObject != NULL)
      {
        const Float minDistance = 2.0f * (radius + m_TargetObject->GetRadius());
        const Float minSpeed = m_Creature.GetMaxLinearSpeed() / 2.0f;
        const Float distanceSqr = SeekEval::GetDistanceSqr(m_Creature, *m_TargetObject);
        if(distanceSqr < Square(minDistance) && SeekEval::GetTotalLinSpeedSqr(m_Creature) < Square(minSpeed))
        {
          /* creature has small linear speed and target is near to it - ignore obstacles */
          return SpatialDescList();
        }
        else
        {
          IgnoreEntityCenterPredicate2 predicate(&m_Creature, m_TargetObject, position, visibilityRadius);
          return spatialDB.CollectSpatialDesc(position, collectRadius, m_VisiblePlanes, predicate);
        }
      }
      else
      {
        IgnoreEntityCenterPredicate1 predicate(&m_Creature, position, visibilityRadius);
        return spatialDB.CollectSpatialDesc(position, collectRadius, m_VisiblePlanes, predicate);
      }
    }
    else
    {
      return SpatialDescList();
    }
  }

  inline SpatialDescList CreatureMotionSys::GetObstacles() const
  {
    return GetObstacles(m_PredictionTime);
  }
  
  void CreatureMotionSys::ApplyVelocities(const MovableVelocities& state, TimeStep dt)
  {
    HandleMotionChanges(m_PreviousVelocities, state, dt);
    m_Creature.SetLinearVelocity(state.GetLinVelocity());
    m_Creature.SetAngularSpeed(state.GetAngSpeed());
  }
  
  void CreatureMotionSys::HandleMotionChanges(const MovableVelocities& prev,
    const MovableVelocities& curr, Float dt)
  {
    if(m_MotionObserver != NULL)
    {
      MotionDesc motionDesc;
      if(!m_Creature.CheckDying())
      {
        const Vector2F headDirection = m_Creature.GetDirection();
        const Vector2F linVelDelta = curr.GetTotalLinVelocity() - prev.GetTotalLinVelocity();
        const Float headDirLinSpeedDelta = headDirection * linVelDelta;
        const Float headDirLinSpeedAccel = headDirLinSpeedDelta / dt;
        const Float headDirLinSpeed = headDirection * curr.GetEigenLinVelocity();
        const Float angSpeedDelta = curr.GetTotalAngSpeed() - prev.GetTotalAngSpeed();
        const Float angSpeedAccel = angSpeedDelta / dt;
        
        motionDesc.linSpeedFactor = headDirLinSpeed / m_Creature.GetMaxLinearSpeed();
        if(headDirLinSpeedAccel > ZERO)
        {
          motionDesc.linAccelFactor = headDirLinSpeedAccel / m_Creature.GetMaxLinearAccel();
        }
        else
        {
          motionDesc.linAccelFactor = headDirLinSpeedAccel / m_Creature.GetMaxLinearBraking();
        }
        
        motionDesc.ccwSpeedFactor = curr.GetEigenAngSpeed() / m_Creature.GetMaxLinearSpeed();
        if(angSpeedAccel > ZERO)
        {
          motionDesc.ccwAccelFactor = angSpeedAccel / m_Creature.GetMaxAngularAccel();
        }
        else
        {
          motionDesc.ccwAccelFactor = angSpeedAccel / m_Creature.GetMaxAngularBraking();
        }
        
        motionDesc.linSpeedFactor.Clamp(-1.0f, 1.0f);
        motionDesc.linAccelFactor.Clamp(-1.0f, 1.0f);
        motionDesc.ccwSpeedFactor.Clamp(-1.0f, 1.0f);
        motionDesc.ccwAccelFactor.Clamp(-1.0f, 1.0f);
      }
      
      m_MotionObserver->HandleMotionChanges(motionDesc);
    }
  }
}
