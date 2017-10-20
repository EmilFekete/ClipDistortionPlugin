#ifndef __CLIPDISTORTION__
#define __CLIPDISTORTION__

#include "IPlug_include_in_plug_hdr.h"

class ClipDistortion : public IPlug
{
public:
  ClipDistortion(IPlugInstanceInfo instanceInfo);
  ~ClipDistortion();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mGain;
	double mMix;
	double mOutputVolume;
	double mThreshold;
};

#endif
