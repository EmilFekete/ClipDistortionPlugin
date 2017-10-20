#include "ClipDistortion.h"
#include "IPlug_include_in_plug_src.h"
#include "IControl.h"
#include "resource.h"
#include <cmath>

const int kNumPrograms = 1;

enum EParams
{
  kGain = 0,
  kMix,
  kOutputVolume,
  kThreshold,
  kNumParams
};

enum ELayout
{
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,

	kGainX = 0,
	kGainY = 0,
	kThresholdX = 50,
	kThresholdY = 0,
	kMixX = 100,
	kMixY = 0,
	kOutputVolumeX = 150,
	kOutputVolumeY = 0,
		
	kKnobFrames = 60
};

ClipDistortion::ClipDistortion(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
	, mGain { 0.5 }
	, mMix { 1.0 }
	, mThreshold { 1.0}
	, mOutputVolume { 1.0 }

{
  TRACE;

  GetParam(kGain)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%", "Gain", 2.0);
	GetParam(kMix)->InitDouble("Mix", 100., 0., 100.0, 0.01, "%", "Mix", 1.0);
	GetParam(kOutputVolume)->InitDouble("Output", 100.0, 0., 150.0, 0.01, "%", "Output", 2.0);
	GetParam(kThreshold)->InitDouble("Threshold", 100., 0., 100.0, 0.01, "%", "Threshold", 2.0);

  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&COLOR_RED);

  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kGainX, kGainY, kGain, &knob));
	pGraphics->AttachControl(new IKnobMultiControl(this, kMixX, kMixY, kMix, &knob));
	pGraphics->AttachControl(new IKnobMultiControl(this, kOutputVolumeX, kOutputVolumeY, kOutputVolume, &knob));
	pGraphics->AttachControl(new IKnobMultiControl(this, kThresholdX, kThresholdY, kThreshold, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

ClipDistortion::~ClipDistortion() {}

void ClipDistortion::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
		//Aply input Gain
    *out1 = *in1 * mGain;
    *out2 = *in2 * mGain;

		//Limit the signal range for each chanel
		if (std::abs(*out1) > mThreshold) {
			*out1 = std::copysignl(mThreshold, *out1);
		}
		if (std::abs(*out2) > mThreshold) {
			*out2 = std::copysignl(mThreshold,*out2);
		} 

		//Mix with the original signal
		*out1 = (*in1)*(1. - mMix) + (*out1)*mMix;
		*out2 = (*in2)*(1. - mMix) + (*out2)*mMix;

		//Aply output Gain
		*out1 *= mOutputVolume;
		*out2 *= mOutputVolume;
  }
}

void ClipDistortion::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void ClipDistortion::OnParamChange(int paramIdx)
{
  IMutexLock lock(this);

  switch (paramIdx)
  {
    case kGain:
      mGain = GetParam(kGain)->Value() / 100.;
      break;
		case kMix:
			mMix = GetParam(kMix)->Value() / 100.;
			break;
		case kOutputVolume:
			mOutputVolume = GetParam(kOutputVolume)->Value() / 100.;
			break;
		case kThreshold:
			mThreshold = GetParam(kThreshold)->Value() / 100.;
			break;

    default:
      break;
  }
}
