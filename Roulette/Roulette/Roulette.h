#ifndef ROULETTE_13_30_04_H__
#define ROULETTE_13_30_04_H__

/*********************************************************************************************************
  created:      2012/11/16
  file base:    Roulette
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Math/Float.h>
#include <Common/Helpers.h>
#include <Common/Randomizer.h>
#include <Generics/Roulette/Impl/RouletteImpl.h>

namespace core
{
  template<class T>
  class Roulette : core::AssignmentFree
  {
    private:
      struct ValueAux
      {
        ValueAux() {}
        ValueAux(Float probability) : probability(probability) {}
      
        Float probability;
      };
    public:
      DEFINE_ROULETTE_VALUE_TYPE(Value, Roulette, T, ValueAux);

      typedef impl::RouletteImpl<Value> Impl;
      typedef T Object;
      typedef typename Impl::ValuesRange ValuesRange;
      
      void Register(const Object& object, Float probability);
      void SetProbability(const Object& object, Float probability);
      void AddProbability(const Object& object, Float probChange);
      const Object& GetNext() const;
    protected:
      size_t GetAllowedValuesAmount() const;
      void AddProbabilitySum(Float probability);
      Ptr<Value> GetNextValue();
    private:
      typedef typename Impl::Value ImplValue;
      struct ValueAccessor
      {
        Float GetProbability(const Value& value) const { return value.GetAux().probability; }
      };

      void AllowValue(ImplValue* value);
      void BanValue(ImplValue* value);

      Impl m_Impl;
      Float m_ProbabilitySum;
  };
  
  template<class T>
  inline void Roulette<T>::Register(const Object& object, Float probability)
  {
    ENFORCE(probability >= 0.0f);
    const Ptr<Value> value = m_Impl.AddValue();
    value->Initialize(this, object, ValueAux(probability));
    m_ProbabilitySum += probability;
  }
  
  template<class T>
  void Roulette<T>::SetProbability(const Object& object, Float probability)
  {
    ENFORCE(probability >= 0.0f);
    const Ptr<Value> value = m_Impl.FindValue(object);
    ENFORCE(value != NULL);
    ValueAux& aux = value->GetAux();
    const Float probChange = probability - aux.probability;
    aux.probability = probability;
    AddProbabilitySum(probChange);
  }
  
  template<class T>
  void Roulette<T>::AddProbability(const Object& object, Float probChange)
  {
    const Ptr<Value> value = m_Impl.FindValue(object);
    ENFORCE(value != NULL);
    ValueAux& aux = value->GetAux();
    aux.probability += probChange;
    if(aux.probability < 0.0f)
    {
      probChange -= aux.probability;
      aux.probability = 0.0f;
    }
    
    AddProbabilitySum(probChange);
  }
  
  template<class T>
  inline const typename Roulette<T>::Object& Roulette<T>::GetNext() const
  {
    return const_cast<Roulette*>(this)->GetNextValue()->GetObject();
  }

  template<class T>
  inline size_t Roulette<T>::GetAllowedValuesAmount() const
  {
    return m_Impl.GetAllowedValuesAmount();
  }
  
  template<class T>
  inline void Roulette<T>::AddProbabilitySum(Float probability)
  {
    m_ProbabilitySum += probability;
  }
  
  template<class T>
  inline Ptr<typename Roulette<T>::Value> Roulette<T>::GetNextValue()
  {
    const Float rand = core::Randomizer::Static::Generate(m_ProbabilitySum);
    const ValueAccessor valueAccessor = {};
    return m_Impl.GetRandomAllowedValue(rand, valueAccessor);
  }

  template<class T>
  inline void Roulette<T>::AllowValue(ImplValue* value)
  {
    const ValueAux& aux = value->GetAux();
    m_ProbabilitySum += aux.probability;
    m_Impl.AllowValue(value);
  }

  template<class T>
  inline void Roulette<T>::BanValue(ImplValue* value)
  {
    const ValueAux& aux = value->GetAux();
    m_ProbabilitySum -= aux.probability;
    m_Impl.BanValue(value);
  }
}

#endif /* ROULETTE_13_30_04_H__ */

