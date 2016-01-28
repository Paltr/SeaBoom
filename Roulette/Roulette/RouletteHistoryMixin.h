#ifndef ROULETTEHISTORYMIXIN_11_22_42_H__
#define ROULETTEHISTORYMIXIN_11_22_42_H__

/*********************************************************************************************************
  created:      2015/08/01
  file base:    RouletteHistoryMixin
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Generics/List.h>

namespace core
{
  template<class TBaseRoulette>
  class RouletteHistoryMixin : public TBaseRoulette
  {
    public:
      typedef TBaseRoulette Base;
      typedef typename Base::Object Object;
      typedef typename Base::Value Value;
      
      class ObjectProxy
      {
        public:
          ObjectProxy();
          ObjectProxy(RouletteHistoryMixin* owner, Ptr<Value> value);
          void RemoveFromHistory();
          operator const Object&() const;
        private:
          RouletteHistoryMixin* m_Owner;
          Ptr<Value> m_Value;
      };

      RouletteHistoryMixin();
      void SetHistoryLength(size_t historyLength);
      const Object& GetNext() const;
    private:
      typedef core::List<Ptr<Value> > History;

      void RemoveFromHistory(Ptr<Value> value);
      void PopValueFromHistory() const;

      size_t m_HistoryLength;
      mutable History m_History;
  };

  /*************************************************************************************
                           RouletteHistoryMixin::ObjectProxy
  **************************************************************************************/
  template<class TBaseRoulette>
  inline RouletteHistoryMixin<TBaseRoulette>::ObjectProxy::ObjectProxy()
    : m_Owner(NULL)
  {
  }

  template<class TBaseRoulette>
  inline RouletteHistoryMixin<TBaseRoulette>::ObjectProxy::ObjectProxy(RouletteHistoryMixin* owner,
    Ptr<Value> value)
    : m_Owner(owner),
      m_Value(value)
  {
  }

  template<class TBaseRoulette>
  inline void RouletteHistoryMixin<TBaseRoulette>::ObjectProxy::RemoveFromHistory()
  {
    m_Owner->RemoveFromHistory(m_Value);
  }

  template<class TBaseRoulette>
  inline RouletteHistoryMixin<TBaseRoulette>::ObjectProxy::
    operator const typename RouletteHistoryMixin<TBaseRoulette>::Object&() const
  {
    return m_Value->GetObject();
  }

  /*************************************************************************************
                                   Inline methods
  **************************************************************************************/
  template<class TBaseRoulette>
  inline RouletteHistoryMixin<TBaseRoulette>::RouletteHistoryMixin()
    : m_HistoryLength(0)
  {
  }

  template<class TBaseRoulette>
  inline void RouletteHistoryMixin<TBaseRoulette>::SetHistoryLength(size_t historyLength)
  {
    m_HistoryLength = historyLength;
    while(m_History.GetSize() > m_HistoryLength)
    {
      PopValueFromHistory();
    }
  }

  template<class TBaseRoulette>
  inline const typename RouletteHistoryMixin<TBaseRoulette>::Object&
    RouletteHistoryMixin<TBaseRoulette>::GetNext() const
  {
    const Ptr<Value> value = const_cast<RouletteHistoryMixin*>(this)->GetNextValue();
    value->Ban();
    m_History.PushFront(value);
    if(m_HistoryLength != 0)
    {
      if(m_History.GetSize() == m_HistoryLength || this->GetAllowedValuesAmount() == 0)
      {
        PopValueFromHistory();
      }
    }

    return value->GetObject();
  }

  template<class TBaseRoulette>
  inline void RouletteHistoryMixin<TBaseRoulette>::RemoveFromHistory(Ptr<Value> value)
  {
    if(m_History.TryRemove(value))
    {
      value->Allow();
    }
  }

  template<class TBaseRoulette>
  inline void RouletteHistoryMixin<TBaseRoulette>::PopValueFromHistory() const
  {
    const Ptr<Value> value = m_History.Back();
    value->Allow();
    m_History.PopBack();
  }
}

#endif /* ROULETTEHISTORYMIXIN_11_22_42_H__ */

