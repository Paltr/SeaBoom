#ifndef CREATURESHARED_15_27_43_H__
#define CREATURESHARED_15_27_43_H__

/*********************************************************************************************************
  created:      2013/05/23
  file base:    CreatureShared
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <AuxLib/Animable/Animable.h>

namespace game
{
  struct CreatureShared
  {
    typedef core::Vector<aux::TexelCircleInfo> BubbleSpawnInfo;
    
    BubbleSpawnInfo bubbleSpawnInfo;
  };
}

#endif /* CREATURESHARED_15_27_43_H__ */

