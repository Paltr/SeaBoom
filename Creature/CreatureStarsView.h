#ifndef CREATURESTARSVIEW_17_11_13_H__
#define CREATURESTARSVIEW_17_11_13_H__

/*********************************************************************************************************
  created:      2013/07/17
  file base:    CreatureStarsView
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Components/View/IView.h>
#include "CreatureTypes.h"

namespace game
{
  class CreatureStarsView : public IView
  {
    public:
      CreatureStarsView(const Ref<ResourceFactory>& resFactory, const CRef<EnvViewState>& envViewState,
        const Ref<aux::Animable>& animable, aux::AnimableObjectId texelCircleId);
      bool CheckActive() const;
      void Activate();
      void HandleTick(TimeStep dt);
      void Deactivate();
      
      virtual void Render(engine::SceneManager& sceneManager);
    private:
      struct SectorState
      {
        SectorState() : active(false) {}
        
        bool active;
        Float appearDelay;
        Float lifetime;
        Float time;
        core::Angle starAngOffset;
        core::Angle pinAngle;
        core::Angle pinAngSpeed;
        ValueState0112 scaleMult;
        ValueState0112 alphaMult;
      };
      
      typedef core::Vector<SectorState, core::TVectorOpt::Pool> SectorStatesList;
      
      const CRef<EnvViewState> m_EnvViewState;
      const aux::AnimableTexelCircle m_TexelCircle;
      const Ref<engine::Image> m_StarImage;
      bool m_Active;
      bool m_Done;
      core::Angle m_BaseAngle;
      core::Angle m_SectorAngle;
      SectorStatesList m_SectorStatesList;
  };
  
  inline bool CreatureStarsView::CheckActive() const
  {
    return m_Active;
  }
}

#endif /* CREATURESTARSVIEW_17_11_13_H__ */

