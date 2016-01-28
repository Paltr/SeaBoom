#ifndef IMOTIONOBSERVER_09_28_08_H__
#define IMOTIONOBSERVER_09_28_08_H__

/*********************************************************************************************************
  created:      2012/08/30
  file base:    IMotionObserver
  file ext:     h
  author:       Paltr
  
  purpose:      
  history:      
  copyright:    Red Eye Ltd.
*********************************************************************************************************/

#include <Math/Float.h>

namespace game
{
  struct MotionDesc
  {
    Float linSpeedFactor;
    Float linAccelFactor;
    Float ccwSpeedFactor;
    Float ccwAccelFactor;
  };
  
  class IMotionObserver
  {
    public:
      virtual ~IMotionObserver() {}
      virtual void HandleMotionChanges(const MotionDesc& motionDesc) = 0;
    private:
      IMotionObserver& operator = (const IMotionObserver&);
  };
}

#endif /* IMOTIONOBSERVER_09_28_08_H__ */

