#ifndef ROULETTE_20_13_49_H__
#define ROULETTE_20_13_49_H__

/*********************************************************************************************************
  created:      2012/11/16
  file base:    Roulette
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Generics/Roulette/Roulette.h>
#include <Generics/Roulette/PricedObjectRoulette.h>
#include <Generics/Roulette/RouletteHistoryMixin.h>

namespace core
{
  template<class T>
  class RouletteWithHistory : public RouletteHistoryMixin<Roulette<T> >
  {
  };

  template<class T>
  class PricedObjectRouletteWithHistory : public RouletteHistoryMixin<PricedObjectRoulette<T> >
  {
  };
}

#endif /* ROULETTE_20_13_49_H__ */

