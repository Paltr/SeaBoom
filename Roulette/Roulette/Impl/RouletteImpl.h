#ifndef ROULETTEIMPL_13_11_52_H__
#define ROULETTEIMPL_13_11_52_H__

/*********************************************************************************************************
  created:      2012/11/16
  file base:    RouletteImpl
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Common/Helpers.h>
#include <Generics/Reference.h>
#include <Generics/List.h>
#include <Generics/Roulette/Impl/RouletteValueBase.h>

namespace core
{
  namespace impl
  {
    template<class TValue>
    class RouletteImpl
    {
      public:
        typedef TValue Value;
        typedef typename Value::Object Object;
        typedef typename Value::AuxData AuxData;
        typedef IntrusiveList<Value> ValuesList;
        typedef List<Value> ValuesStorage;
      
        class ValuesCRange
        {
          public:
            ValuesCRange(const typename ValuesList::Range& range);
            bool Empty() const;
            void PopFront();
            CPtr<Value> Front() const;
          
            INJECT_CRANGE_OPERATORS(CPtr<Value>);
          protected:
            typename ValuesList::Range m_Range;
        };
      
        class ValuesRange : public ValuesCRange
        {
          public:
            typedef ValuesCRange Base;
          
            ValuesRange(const typename ValuesList::Range& range);
          
            using ValuesCRange::Front;
            Ptr<Value> Front();
          
            INJECT_RANGE_OPERATORS(CPtr<Value>, Ptr<Value>);
        };
      
        Ptr<Value> AddValue();

        Ptr<Value> FindValue(const Object& object);
        CPtr<Value> FindValue(const Object& object) const;

        const ValuesStorage& GetValuesStorage() const;

        void AllowValue(Value* value);
        void BanValue(Value* value);

        size_t GetAllowedValuesAmount() const;
        ValuesCRange GetAllowedValues() const;
        ValuesRange GetAllowedValues();

        size_t GetBannedValuesAmount() const;
        ValuesCRange GetBannedValues() const;
        ValuesRange GetBannedValues();

        template<class TValueAccessor>
        Ptr<Value> GetRandomAllowedValue(Float rand, const TValueAccessor& valueAccessor);
      private:
        ValuesStorage m_ValuesStorage;
        ValuesList m_AllowedValues;
        ValuesList m_BannedValues;
    };

    /*************************************************************************************
                                   RouletteImpl::ValuesCRange
    **************************************************************************************/
    template<class TValue>
    inline RouletteImpl<TValue>::ValuesCRange::ValuesCRange(const typename ValuesList::Range& range)
      : m_Range(range)
    {
    }
  
    template<class TValue>
    inline bool RouletteImpl<TValue>::ValuesCRange::Empty() const
    {
      return m_Range.Empty();
    }
  
    template<class TValue>
    inline void RouletteImpl<TValue>::ValuesCRange::PopFront()
    {
      m_Range.PopFront();
    }
  
    template<class TValue>
    inline CPtr<typename RouletteImpl<TValue>::Value> RouletteImpl<TValue>::ValuesCRange::Front() const
    {
      return m_Range.Front();
    }
  
    /*************************************************************************************
                                  RouletteImpl::ValuesRange
    **************************************************************************************/
    template<class TValue>
    inline RouletteImpl<TValue>::ValuesRange::ValuesRange(const typename ValuesList::Range& range)
      : Base(range)
    {
    }
  
    template<class TValue>
    inline Ptr<typename RouletteImpl<TValue>::Value> RouletteImpl<TValue>::ValuesRange::Front()
    {
      return this->m_Range.Front();
    }

    /*************************************************************************************
                                     Inline methods
    **************************************************************************************/
    template<class TValue>
    inline Ptr<typename RouletteImpl<TValue>::Value> RouletteImpl<TValue>::AddValue()
    {
      Value& value = m_ValuesStorage.EmplaceBack();
      m_AllowedValues.PushBack(&value);
      return &value;
    }
  
    template<class TValue>
    inline Ptr<typename RouletteImpl<TValue>::Value> RouletteImpl<TValue>::FindValue(const Object& object)
    {
      typename ValuesStorage::Range range = m_ValuesStorage.GetRange();
      while(range)
      {
        Value& value = *range;
        if(value.GetObject() == object)
        {
          return &value;
        }
      
        range.PopFront();
      }
    
      return Ptr<Value>();
    }
  
    template<class TValue>
    inline CPtr<typename RouletteImpl<TValue>::Value>
      RouletteImpl<TValue>::FindValue(const Object& object) const
    {
      return const_cast<RouletteImpl>(this)->FindValue(object);
    }
  
    template<class TValue>
    inline const typename RouletteImpl<TValue>::ValuesStorage& RouletteImpl<TValue>::GetValuesStorage() const
    {
      return m_ValuesStorage;
    }

    template<class TValue>
    inline void RouletteImpl<TValue>::AllowValue(Value* value)
    {
      m_BannedValues.Remove(value);
      m_AllowedValues.PushBack(value);
    }

    template<class TValue>
    inline void RouletteImpl<TValue>::BanValue(Value* value)
    {
      m_AllowedValues.Remove(value);
      m_BannedValues.PushBack(value);
    }
  
    template<class TValue>
    inline size_t RouletteImpl<TValue>::GetAllowedValuesAmount() const
    {
      return m_AllowedValues.GetSize();
    }
  
    template<class TValue>
    inline typename RouletteImpl<TValue>::ValuesCRange RouletteImpl<TValue>::GetAllowedValues() const
    {
      return ValuesCRange(const_cast<ValuesList&>(m_AllowedValues).GetRange());
    }
  
    template<class TValue>
    inline typename RouletteImpl<TValue>::ValuesRange RouletteImpl<TValue>::GetAllowedValues()
    {
      return ValuesRange(m_AllowedValues.GetRange());
    }
  
    template<class TValue>
    inline size_t RouletteImpl<TValue>::GetBannedValuesAmount() const
    {
      return m_BannedValues.GetSize();
    }
  
    template<class TValue>
    inline typename RouletteImpl<TValue>::ValuesCRange RouletteImpl<TValue>::GetBannedValues() const
    {
      return ValuesCRange(const_cast<ValuesList&>(m_BannedValues).GetRange());
    }
  
    template<class TValue>
    inline typename RouletteImpl<TValue>::ValuesRange RouletteImpl<TValue>::GetBannedValues()
    {
      return ValuesRange(m_BannedValues.GetRange());
    }

    template<class TValue>
    template<class TValueAccessor>
    Ptr<TValue> RouletteImpl<TValue>::GetRandomAllowedValue(Float rand, const TValueAccessor& valueAccessor)
    {
      ValuesRange range = this->GetAllowedValues();
      Float randSum;
      while(range)
      {
        const Ptr<Value> value = *range;
        randSum += valueAccessor.GetProbability(*value);
        if(rand <= randSum)
        {
          return value;
        }
      
        range.PopFront();
      }
    
      ENFORCE(false)("Unreachable code");
      return Ptr<Value>();
    }
  }
}

#endif /* ROULETTEIMPL_13_11_52_H__ */

