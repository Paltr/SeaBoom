#ifndef CREATURESTARSVIEW_18_20_10_H__
#define CREATURESTARSVIEW_18_20_10_H__

/*********************************************************************************************************
  created:      2013/07/17
  file base:    CreatureStarsView
  file ext:     cpp
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Common/Randomizer.h>
#include "CreatureStarsView.h"
#include <Game.h>

using namespace core;
using namespace engine;
using namespace aux;

namespace game
{
  const Float BASE_ANG_SPEED = Math::Deg2Rad(180.0f);
  
  const Float STAR_INIT_DELAY_MAX = 0.1f;
  const Float STAR_DELAY_MIN = 0.0f;
  const Float STAR_DELAY_MAX = 0.1f;
  const Float STAR_LIFETIME_MIN = 1.0f;
  const Float STAR_LIFETIME_MAX = 1.3f;
  const Float STAR_ANG_SPEED_MIN = Math::Deg2Rad(90.0f);
  const Float STAR_ANG_SPEED_MAX = Math::Deg2Rad(120.0f);
  
  const Float STAR_SIZE_0 = 0.1f;
  const Float STAR_SIZE_1 = 1.0f;
  const Float STAR_SIZE_2 = 0.1f;
  
  const Float STAR_SIZE_INC_END_01 = 0.4f;
  const Float STAR_SIZE_DEC_START_01 = 0.6f;
  
  const Float STAR_ALPHA_INC_END_01 = 0.2f;
  const Float STAR_ALPHA_DEC_START_01 = 0.8f;
  
  const Float STAR_SIZE_DURATION_01 = STAR_SIZE_INC_END_01;
  const Float STAR_SIZE_DURATION_11 = STAR_SIZE_DEC_START_01 - STAR_SIZE_INC_END_01;
  const Float STAR_SIZE_DURATION_12 = 1.0f - STAR_SIZE_DEC_START_01;
  
  const Float STAR_ALPHA_DURATION_01 = STAR_ALPHA_INC_END_01;
  const Float STAR_ALPHA_DURATION_11 = STAR_ALPHA_DEC_START_01 - STAR_ALPHA_INC_END_01;
  const Float STAR_ALPHA_DURATION_12 = 1.0f - STAR_ALPHA_DEC_START_01;
  
  CreatureStarsView::CreatureStarsView(const Ref<ResourceFactory>& resFactory,
    const CRef<EnvViewState>& envViewState, const Ref<Animable>& animable, AnimableObjectId texelCircleId)
    : IView(RenderStage::_4_POST_CREATURES_W_EFFECT_TRANSP),
      m_EnvViewState(envViewState),
      m_TexelCircle(AnimableTexelCircle::CreateIgnoreInvalidObjectId(animable, texelCircleId)),
      m_StarImage(resFactory->CreateImage(HEAD_STAR)),
      m_Active(false),
      m_Done(false)
  {
  }
  
  void CreatureStarsView::Activate()
  {
    m_Active = true;
    m_Done = false;
    m_BaseAngle = Randomizer::Static::Generate(ANGLE_20_PI);
    const OriCircle& circle = m_TexelCircle.GetCircle();
    const Float starSize = STAR_SIZE_1 * m_EnvViewState->screenToWorldScale * Max(m_StarImage->GetSize());
    const Float sin = starSize / (2.0f * circle.radius);
    m_SectorAngle = 2.0f * Angle::ASin(sin);
    const size_t sectorsAmount = (ANGLE_20_PI / m_SectorAngle).ToSize();
    m_SectorAngle = ANGLE_20_PI / (Float)sectorsAmount;
    m_SectorStatesList.Resize(sectorsAmount);
    for(size_t index = 0; index < sectorsAmount; ++index)
    {
      SectorState& state = m_SectorStatesList[index];
      state.active = false;
      state.appearDelay = Randomizer::Static::Generate(STAR_INIT_DELAY_MAX);
    }
  }
  
  void CreatureStarsView::HandleTick(TimeStep dt)
  {
    if(!m_Done)
    {
      m_Done = !m_Active;
      m_BaseAngle += dt * BASE_ANG_SPEED;
      const size_t sectorsAmount = m_SectorStatesList.GetSize();
      for(size_t index = 0; index < sectorsAmount; ++index)
      {
        SectorState& state = m_SectorStatesList[index];
        if(state.active)
        {
          m_Done = false;
          state.time += dt;
          if(state.time > state.lifetime)
          {
            state.active = false;
            state.appearDelay = Randomizer::Static::Generate(STAR_DELAY_MIN, STAR_DELAY_MAX);
          }
          else
          {
            state.pinAngle += dt * state.pinAngSpeed;
          }
        }
        else if(m_Active)
        {
          state.appearDelay -= dt;
          if(state.appearDelay <= 0.0f)
          {
            state.active = true;
            state.lifetime = Randomizer::Static::Generate(STAR_LIFETIME_MIN, STAR_LIFETIME_MAX);
            state.time = 0.0f;
            state.starAngOffset = (Float)index * m_SectorAngle + Randomizer::Static::Generate(m_SectorAngle);
            state.pinAngle = Randomizer::Static::Generate(ANGLE_20_PI);
            state.pinAngSpeed = Randomizer::Static::Generate(STAR_ANG_SPEED_MIN, STAR_ANG_SPEED_MAX);
            
            state.scaleMult.v0 = STAR_SIZE_0;
            state.scaleMult.duration01 = STAR_SIZE_DURATION_01 * state.lifetime;
            state.scaleMult.v1 = STAR_SIZE_1;
            state.scaleMult.duration11 = STAR_SIZE_DURATION_11 * state.lifetime;
            state.scaleMult.v2 = STAR_SIZE_2;
            state.scaleMult.duration12 = STAR_SIZE_DURATION_12 * state.lifetime;
            
            state.alphaMult.v0 = 0.0f;
            state.alphaMult.duration01 = STAR_ALPHA_DURATION_01 * state.lifetime;
            state.alphaMult.v1 = 1.0f;
            state.alphaMult.duration11 = STAR_ALPHA_DURATION_11 * state.lifetime;
            state.alphaMult.v2 = 0.0f;
            state.alphaMult.duration12 = STAR_ALPHA_DURATION_12 * state.lifetime;
          }
        }
      }
    }
  }
  
  void CreatureStarsView::Deactivate()
  {
    m_Active = false;
  }
  
  /* virtual */
  void CreatureStarsView::Render(SceneManager& sceneManager)
  {
    const BlitTexModeSetter blitTexModeSetter(sceneManager);
    sceneManager.SetGeometrySource(GEOM_SRC_IMAGE_QUAD);
    sceneManager.SetFrame(m_StarImage);
    const Ref<SetAuxFloatProg>& vertexProg =
      sceneManager.GetPresetVertexProgram(VERTEX_PROGRAM_SET_AUX_FLOAT);
    const VertexProgramSetter vertexProgramSetter(sceneManager, vertexProg);
    
    const OriCircle& circle = m_TexelCircle.GetCircle();
    SectorStatesList::CRange range = m_SectorStatesList.GetRange();
    while(range)
    {
      const SectorState& state = *range;
      if(state.active)
      {
        const Angle angle = m_BaseAngle + state.starAngOffset;
        const Vector2F position = circle.center + Vector2F(circle.radius, 0.0f).Rotate(angle);
        const Angle pinAngle = state.pinAngle;
        const Float scale = state.scaleMult.GetValue(state.time);
        vertexProg->Setup(state.alphaMult.GetValue(state.time));
        sceneManager.Render(Transform(position, pinAngle, Vector2F(scale, scale)), Anchor::Center());
      }
      
      range.PopFront();
    }
  }
}

#endif /* CREATURESTARSVIEW_18_20_10_H__ */

