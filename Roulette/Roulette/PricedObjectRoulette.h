#ifndef PRICEDOBJECTROULETTE_11_36_02_H__
#define PRICEDOBJECTROULETTE_11_36_02_H__

/*********************************************************************************************************
  created:      2015/08/01
  file base:    PricedObjectRoulette
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Math/Float.h>
#include <Common/Helpers.h>
#include <Common/Randomizer.h>
#include <Generics/Vector.h>
#include <Generics/Roulette/Impl/RouletteImpl.h>

namespace core
{
  template<class T>
  class PricedObjectRoulette : core::AssignmentFree
  {
    private:
      struct ValueAux
      {
        ValueAux() {}
        ValueAux(Float probability, Float points) : probability(probability), points(points) {}
      
        Float probability;
        Float points;
      };
    public:
      DEFINE_ROULETTE_VALUE_TYPE(Value, PricedObjectRoulette, T, ValueAux);

      typedef impl::RouletteImpl<Value> Impl;
      typedef T Object;
      typedef typename Impl::ValuesRange ValuesRange;
      
      bool CheckSolvent() const;
      Float GetPointsSum() const;
      void SetPointsAmount(Float pointsAmount);
      void Register(const Object& object, Float probability, Float points);
      const Object& GetNext() const;
    protected:
      size_t GetAllowedValuesAmount() const;
      Ptr<Value> GetNextValue();
    private:
      typedef Vector<Ptr<Value> > ExpensiveValuesList;
      struct ValueAccessor
      {
        Float GetProbability(const Value& value) const { return value.GetAux().probability; }
      };

      void ExcludeExpensiveObjects();

      void AllowValue(Value* value);
      void BanValue(Value* value);

      Impl m_Impl;
      Float m_ProbabilitySum;
      Float m_PointsSum;
      Float m_PointsAmount;
      ExpensiveValuesList m_ExpensiveValuesList;
  };

  template<class T>
  inline bool PricedObjectRoulette<T>::CheckSolvent() const
  {
    return m_Impl.GetAllowedValuesAmount() != 0;
  }

  template<class T>
  inline Float PricedObjectRoulette<T>::GetPointsSum() const
  {
    return m_PointsSum;
  }

  template<class T>
  inline void PricedObjectRoulette<T>::SetPointsAmount(Float pointsAmount)
  {
    m_PointsAmount = pointsAmount;
    typename ExpensiveValuesList::Range range = m_ExpensiveValuesList.GetRange();
    while(range)
    {
      const Ptr<Value> value = *range;
      value->Allow();
      range.PopFront();
    }

    m_ExpensiveValuesList.Clear();
    ExcludeExpensiveObjects();
  }

  template<class T>
  inline void PricedObjectRoulette<T>::Register(const Object& object, Float probability, Float points)
  {
    ENFORCE(probability >= 0.0f);
    ENFORCE(points >= 0.0f);
    const Ptr<Value> value = m_Impl.AddValue();
    value->Initialize(this, object, ValueAux(probability, points));
    m_ProbabilitySum += probability;
    m_PointsSum += points;
  }

  template<class T>
  inline const typename PricedObjectRoulette<T>::Object& PricedObjectRoulette<T>::GetNext() const
  {
    return const_cast<PricedObjectRoulette*>(this)->GetNextValue()->GetObject();
  }

  template<class T>
  inline size_t PricedObjectRoulette<T>::GetAllowedValuesAmount() const
  {
    return m_Impl.GetAllowedValuesAmount();
  }

  template<class T>
  inline Ptr<typename PricedObjectRoulette<T>::Value> PricedObjectRoulette<T>::GetNextValue()
  {
    const Float rand = core::Randomizer::Static::Generate(m_ProbabilitySum);
    const ValueAccessor valueAccessor;
    const Ptr<Value> value = m_Impl.GetRandomAllowedValue(rand, valueAccessor);
    m_PointsAmount -= value->GetAux().points;
    ENFORCE(m_PointsAmount >= 0.0f);
    ExcludeExpensiveObjects();
    return value;
  }

  template<class T>
  inline void PricedObjectRoulette<T>::ExcludeExpensiveObjects()
  {
    ValuesRange range = m_Impl.GetAllowedValues();
    while(range)
    {
      const Ptr<Value> value = *range;
      const Float points = value->GetAux().points;
      range.PopFront();
      if(points > m_PointsAmount)
      {
        value->Ban();
        m_ExpensiveValuesList.PushBack(value);
      }
    }
  }

  template<class T>
  inline void PricedObjectRoulette<T>::AllowValue(Value* value)
  {
    const ValueAux& aux = value->GetAux();
    m_ProbabilitySum += aux.probability;
    m_Impl.AllowValue(value);
  }

  template<class T>
  inline void PricedObjectRoulette<T>::BanValue(Value* value)
  {
    const ValueAux& aux = value->GetAux();
    m_ProbabilitySum -= aux.probability;
    m_Impl.BanValue(value);
  }
}

#endif /* PRICEDOBJECTROULETTE_11_36_02_H__ */

