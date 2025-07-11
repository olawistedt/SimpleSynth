#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include "dsp/Voice.h"
#include "dsp/DetunedSawOscillator.h" 

const int kNumPresets = 1;
const int kNumVoices = 10;

enum EParams
{
  kParamGain = 0,
  kParamNoteGlideTime,
  kParamAttack,
  kParamDecay,
  kParamSustain,
  kParamRelease,
  kParamLFOShape,
  kParamLFORateHz,
  kParamLFORateTempo,
  kParamLFORateMode,
  kParamLFODepth,
  kParamDetuneAmount1,
  kParamDetuneAmount2,
  kParamMixOscillators,
  kParamOsc2Semitone,
  kNumParams
};

#if IPLUG_DSP
// will use EParams in SimpleSynth_DSP.h
#include "SimpleSynth_DSP.h"
#endif

enum EControlTags
{
  kCtrlTagMeter = 0,
  kCtrlTagLFOVis,
  kCtrlTagScope,
  kCtrlTagRTText,
  kCtrlTagKeyboard,
  kCtrlTagBender,
  kNumCtrlTags
};

using namespace iplug;
using namespace igraphics;

class SimpleSynth final : public Plugin
{
public:
  SimpleSynth(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
public:
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void ProcessMidiMsg(const IMidiMsg& msg) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  void OnParamChangeUI(int paramIdx, EParamSource source) override;
  void OnIdle() override;
  bool OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData) override;

private:
  IMidiQueue mMidiQueue;
  Voice mVoice[kNumVoices];
  short mVoices[kNumVoices];

  SimpleSynthDSP<sample> mDSP {16};
  IPeakAvgSender<2> mMeterSender;
  ISender<1> mLFOVisSender;
#endif
};
