#ifndef ROULETTEVALUEBASE_07_05_54_H__
#define ROULETTEVALUEBASE_07_05_54_H__

/*********************************************************************************************************
  created:      2015/07/31
  file base:    RouletteValueBase
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Generics/IntrusiveList.h>

namespace core
{
  namespace impl
  {
    template<class TRoulette, class T, class TAux>
    class RouletteValueBase
    {
      public:
        typedef T Object;
        typedef TAux AuxData;

        RouletteValueBase();

        const T& GetObject() const;
        TAux& GetAux();
        const TAux& GetAux() const;
      protected:
        void Initialize(TRoulette* roulette, const T& object, const TAux& aux);

        TRoulette* m_Roulette;
        T m_Object;
        TAux m_Aux;
    };

    /*************************************************************************************
                                     Inline methods
    **************************************************************************************/
    template<class TRoulette, class T, class TAux>
    inline RouletteValueBase<TRoulette, T, TAux>::RouletteValueBase()
      : m_Roulette(NULL)
    {
    }
  
    template<class TRoulette, class T, class TAux>
    inline void RouletteValueBase<TRoulette, T, TAux>::Initialize(
      TRoulette* roulette, const T& object, const TAux& aux)
    {
      m_Roulette = roulette;
      m_Object = object;
      m_Aux = aux;
    }

    template<class TRoulette, class T, class TAux>
    inline const T& RouletteValueBase<TRoulette, T, TAux>::GetObject() const
    {
      return m_Object;
    }
  
    template<class TRoulette, class T, class TAux>
    inline TAux& RouletteValueBase<TRoulette, T, TAux>::GetAux()
    {
      return m_Aux;
    }
  
    template<class TRoulette, class T, class TAux>
    inline const TAux& RouletteValueBase<TRoulette, T, TAux>::GetAux() const
    {
      return m_Aux;
    }
  }
}

#define DEFINE_ROULETTE_VALUE_TYPE(NAME, ROULETTE_TYPE, OBJECT_TYPE, AUX_TYPE)                              \
  class NAME : public core::impl::RouletteValueBase<ROULETTE_TYPE, OBJECT_TYPE, AUX_TYPE>,                  \
               public core::IntrusiveNodeBase<NAME>                                                         \
  {                                                                                                         \
    friend class ROULETTE_TYPE;                                                                             \
    private:                                                                                                \
      typedef core::impl::RouletteValueBase<ROULETTE_TYPE, OBJECT_TYPE, AUX_TYPE> BaseValue;                \
      void Initialize(ROULETTE_TYPE* roulette, const OBJECT_TYPE& object, const AUX_TYPE& aux)              \
      {                                                                                                     \
        BaseValue::Initialize(roulette, object, aux);                                                       \
      }                                                                                                     \
    public:                                                                                                 \
      NAME()                                                                                                \
        : m_BanCounter(0)                                                                                   \
      {                                                                                                     \
      }                                                                                                     \
      void Allow()                                                                                          \
      {                                                                                                     \
        ENFORCE(m_BanCounter > 0);                                                                          \
        if(--m_BanCounter == 0)                                                                             \
        {                                                                                                   \
          this->m_Roulette->AllowValue(this);                                                               \
        }                                                                                                   \
      }                                                                                                     \
      void Ban()                                                                                            \
      {                                                                                                     \
        if(m_BanCounter++ == 0)                                                                             \
        {                                                                                                   \
          this->m_Roulette->BanValue(this);                                                                 \
        }                                                                                                   \
      }                                                                                                     \
    private:                                                                                                \
      size_t m_BanCounter;                                                                                  \
  };

#endif /* ROULETTEVALUEBASE_07_05_54_H__ */

