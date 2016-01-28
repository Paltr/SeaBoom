#ifndef ICREATUREVIEW_18_18_05_H__
#define ICREATUREVIEW_18_18_05_H__

/*********************************************************************************************************
  created:      2013/05/30
  file base:    ICreatureView
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Components/View/ISmoothAnimView.h>
#include "CreatureTypes.h"

namespace game
{
  class ICreatureView : public ISmoothAnimView
  {
    public:
      ICreatureView(const CreatureViewArgs& args, SmoothPriority priority = SmoothPriority::CREATURES)
        : ISmoothAnimView(priority, args.animable)
      {
      }
  };
}

#endif /* ICREATUREVIEW_18_18_05_H__ */

