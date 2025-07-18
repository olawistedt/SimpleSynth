#include "SimpleSynth.h"
#include "IPlug_include_in_plug_src.h"
#include "LFO.h"

SimpleSynth::SimpleSynth(const InstanceInfo& info)
: iplug::Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  for (int i = 0; i < 10; i++)
  {
    mVoices[i] = kFree;
  }

  GetParam(kParamGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
  GetParam(kParamNoteGlideTime)->InitMilliseconds("Note Glide Time", 0., 0.0, 30.);
  GetParam(kParamLFOShape)->InitEnum("LFO Shape", LFO<>::kTriangle, {LFO_SHAPE_VALIST});
  GetParam(kParamLFORateHz)->InitFrequency("LFO Rate", 1., 0.01, 40.);
  GetParam(kParamLFORateTempo)->InitEnum("LFO Rate", LFO<>::k1, {LFO_TEMPODIV_VALIST});
  GetParam(kParamLFORateMode)->InitBool("LFO Sync", true);
  GetParam(kParamLFODepth)->InitPercentage("LFO Depth");


  GetParam(kParamDetuneAmount1)->InitDouble("Detune amount osc 1", 1.0, .0, 50.0, .5, "cents");
  GetParam(kParamDetuneAmount2)->InitDouble("Detune amount osc 2", 2.0, .0, 50.0, .5, "cents");
  GetParam(kParamMixOscillators)->InitDouble("Mix oscillators", .5, 0.0, 1.0, .01, "blend");
  GetParam(kParamOsc2Semitone)->InitInt("Frequency Osc 2", -12, -12, 12, "semitones");
  
  GetParam(kParamVolumeAttack)->InitDouble("Volume Attack", 400., 1., 1000., 0.1, "ms");
  GetParam(kParamVolumeDecay)->InitDouble("Volume Decay", 400., 1., 1000., 0.1, "ms");
  GetParam(kParamVolumeSustain)->InitDouble("Volume Sustain", 0.5, 0., 1., 0.01, "%");
  GetParam(kParamVolumeRelease)->InitDouble("Volume Release", 500., 0., 1000., 10., "ms");

  GetParam(kParamFilterAttack)->InitDouble("Filter Attack", 400., 1., 1000., 0.1, "ms");
  GetParam(kParamFilterDecay)->InitDouble("Filter Decay", 400., 1., 1000., 0.1, "ms");
  GetParam(kParamFilterSustain)->InitDouble("Filter Sustain", 0.5, 0., 1., 0.01, "%");
  GetParam(kParamFilterRelease)->InitDouble("Filter Release", 500., 0., 1000., 10., "ms");

  GetParam(kParamFilterCutoff)->InitDouble("Filter Cutoff",
                   2000.0,
                   20.,
                   22050.0,
                   100.0,
                   "hz",
                   0,
                   "",
                   iplug::IParam::ShapePowCurve(3.5));
  GetParam(kParamFilterResonance)->InitDouble("Filter Resonance", 0.0, 0.0, 1.0, 0.01, "%");
  GetParam(kParamFilterEnvelopeAmount)->InitDouble("Envelope amount", 0.5, 0.0, 1.0, 0.01, "%");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
//    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->EnableMouseOver(true);
    pGraphics->EnableMultiTouch(true);
    
#ifdef OS_WEB
    pGraphics->AttachPopupMenuControl();
#endif

    //    pGraphics->EnableLiveEdit(true);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    // Background
    pGraphics->LoadBitmap(BACKGROUND_FN, 1, true);
    pGraphics->AttachBackground(BACKGROUND_FN);

    const IRECT b = pGraphics->GetBounds().GetPadded(-20.f);
    const IRECT lfoPanel = b.GetFromLeft(300.f).GetFromTop(200.f);
    IRECT keyboardBounds = b.GetFromBottom(100);

    IRECT wheelsBounds = keyboardBounds.ReduceFromLeft(100.f).GetPadded(-10.f);
    pGraphics->AttachControl(new IVKeyboardControl(keyboardBounds, 12, 80), kCtrlTagKeyboard);
    pGraphics->AttachControl(new IWheelControl(wheelsBounds.FracRectHorizontal(0.5)),
                             kCtrlTagBender);
    pGraphics->AttachControl(new IWheelControl(wheelsBounds.FracRectHorizontal(0.5, true), IMidiMsg::EControlChangeMsg::kModWheel));
//    pGraphics->AttachControl(new IVMultiSliderControl<4>(b.GetGridCell(0, 2, 2).GetPadded(-30), "", DEFAULT_STYLE, kParamAttack, EDirection::Vertical, 0.f, 1.f));
    const IRECT controls = b.GetGridCell(1, 2, 2);
//    pGraphics->AttachControl(new IVKnobControl(controls.GetGridCell(0, 2, 6).GetCentredInside(90), kParamGain, "Gain"));
//    pGraphics->AttachControl(new IVKnobControl(controls.GetGridCell(1, 2, 6).GetCentredInside(90), kParamNoteGlideTime, "Glide"));
    pGraphics->AttachControl(
        new IVSliderControl(IRECT(750, 150, 800, 280), kParamVolumeAttack, "A"));
    pGraphics->AttachControl(
        new IVSliderControl(IRECT(800, 150, 850, 280), kParamVolumeDecay, "D"));
    pGraphics->AttachControl(
        new IVSliderControl(IRECT(850, 150, 900, 280), kParamVolumeSustain, "S"));
    pGraphics->AttachControl(
        new IVSliderControl(IRECT(900, 150, 950, 280), kParamVolumeRelease, "R"));


//    pGraphics->AttachControl(new IVLEDMeterControl<2>(controls.GetFromRight(100).GetPadded(-30)), kCtrlTagMeter);
    
    //pGraphics->AttachControl(new IVKnobControl(lfoPanel.GetGridCell(0, 0, 2, 3).GetCentredInside(60), kParamLFORateHz, "Rate"), kNoTag, "LFO")->Hide(true);
    //pGraphics->AttachControl(new IVKnobControl(lfoPanel.GetGridCell(0, 0, 2, 3).GetCentredInside(60), kParamLFORateTempo, "Rate"), kNoTag, "LFO")->DisablePrompt(false);
    //pGraphics->AttachControl(new IVKnobControl(lfoPanel.GetGridCell(0, 1, 2, 3).GetCentredInside(60), kParamLFODepth, "Depth"), kNoTag, "LFO");
    //pGraphics->AttachControl(new IVKnobControl(lfoPanel.GetGridCell(0, 2, 2, 3).GetCentredInside(60), kParamLFOShape, "Shape"), kNoTag, "LFO")->DisablePrompt(false);
    //pGraphics->AttachControl(new IVSlideSwitchControl(lfoPanel.GetGridCell(1, 0, 2, 3).GetFromTop(30).GetMidHPadded(20), kParamLFORateMode, "Sync", DEFAULT_STYLE.WithShowValue(false).WithShowLabel(false).WithWidgetFrac(0.5f).WithDrawShadows(false), false), kNoTag, "LFO");
    //pGraphics->AttachControl(new IVDisplayControl(lfoPanel.GetGridCell(1, 1, 2, 3).Union(lfoPanel.GetGridCell(1, 2, 2, 3)), "", DEFAULT_STYLE, EDirection::Horizontal, 0.f, 1.f, 0.f, 1024), kCtrlTagLFOVis, "LFO");
    
    pGraphics->AttachControl(new IVGroupControl("LFO", "LFO", 10.f, 20.f, 10.f, 10.f));

    // My synth begins
    pGraphics->AttachControl(new IVKnobControl(IRECT(50, 50, 120, 150),
                                               kParamDetuneAmount1,
                                               "Detune 1"));
    pGraphics->AttachControl(
        new IVKnobControl(IRECT(50, 150, 120, 250),
                                               kParamDetuneAmount2,
                                               "Detune 2"));
    pGraphics->AttachControl(
        new IVSliderControl(IRECT(200, 50, 350, 250),
                                                 kParamMixOscillators,
                                                 "Mix oscillators"));
    pGraphics->AttachControl(
        new IVKnobControl(IRECT(130, 150, 200, 250), kParamOsc2Semitone, "Frequency"));

    pGraphics->AttachControl(
        new IVKnobControl(IRECT(350, 50, 420, 150), kParamFilterCutoff, "Cutoff"));
    pGraphics->AttachControl(
            new IVKnobControl(IRECT(420, 50, 490, 150), kParamFilterResonance, "Resonance"));

    pGraphics->AttachControl(
        new IVKnobControl(IRECT(500, 50, 570, 150), kParamFilterEnvelopeAmount, "Env amount"));

    pGraphics->AttachControl(new IVSliderControl(IRECT(350, 150, 400, 280),
                                                 kParamFilterAttack,
                                                 "A"));
    pGraphics->AttachControl(new IVSliderControl(IRECT(400, 150, 450, 280),
                                                 kParamFilterDecay,
                                                 "D"));
    pGraphics->AttachControl(new IVSliderControl(IRECT(450, 150, 500, 280),
                                                 kParamFilterSustain,
                                                 "S"));
    pGraphics->AttachControl(new IVSliderControl(IRECT(500, 150, 550, 280),
                                                 kParamFilterRelease,
                                                 "R"));
    
    pGraphics->AttachControl(new IVButtonControl(keyboardBounds.GetFromTRHC(200, 30).GetTranslated(0, -30), SplashClickActionFunc,
      "Show/Hide Keyboard", DEFAULT_STYLE.WithColor(kFG, COLOR_WHITE).WithLabelText({15.f, EVAlign::Middle})))->SetAnimationEndActionFunction(
      [pGraphics](IControl* pCaller) {
        static bool hide = false;
        pGraphics->GetControlWithTag(kCtrlTagKeyboard)->Hide(hide = !hide);
        pGraphics->Resize(PLUG_WIDTH, hide ? PLUG_HEIGHT / 2 : PLUG_HEIGHT, pGraphics->GetDrawScale());
    });
//#ifdef OS_IOS
//    if(!IsOOPAuv3AppExtension())
//    {
//      pGraphics->AttachControl(new IVButtonControl(b.GetFromTRHC(100, 100), [pGraphics](IControl* pCaller) {
//                               dynamic_cast<IGraphicsIOS*>(pGraphics)->LaunchBluetoothMidiDialog(pCaller->GetRECT().L, pCaller->GetRECT().MH());
//                               SplashClickActionFunc(pCaller);
//                             }, "BTMIDI"));
//    }
//#endif
    
    pGraphics->SetQwertyMidiKeyHandlerFunc([pGraphics](const IMidiMsg& msg) {
                                              pGraphics->GetControlWithTag(kCtrlTagKeyboard)->As<IVKeyboardControl>()->SetNoteFromMidi(msg.NoteNumber(), msg.StatusMsg() == IMidiMsg::kNoteOn);
                                           });
  };
#endif
}

#if IPLUG_DSP
void SimpleSynth::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{

    // Channel declaration.
  PLUG_SAMPLE_DST *out01 = outputs[0];
  PLUG_SAMPLE_DST *out02 = outputs[1];

  for (int s = 0; s < nFrames; s++)
  {
    while (!mMidiQueue.Empty())
    {
      IMidiMsg msg = mMidiQueue.Peek();
      if (msg.StatusMsg() == IMidiMsg::kNoteOn)
      {
        assert(msg.NoteNumber() != 0);
        short useVoice = -1;
        // Allocate a voice for the note.
        for (int i = 0; i < kNumVoices; ++i)
        {
          if (mVoices[i] == kFree)
          {
            useVoice = i;
            mVoices[i] = msg.NoteNumber();
            break;
          }
        }
        if (useVoice != -1)
        {
          mVoice[useVoice].NoteOn(msg.NoteNumber());
        }
      }
      else if (msg.StatusMsg() == IMidiMsg::kNoteOff)
      {
        short releaseVoiceNr;
        for (int i = 0; i < kNumVoices; ++i)
        {
          if (mVoices[i] == msg.NoteNumber())
          {
            releaseVoiceNr = i;
            mVoices[i] = kInRelease;
            break;
          }
        }
        mVoice[releaseVoiceNr].NoteOff(msg.NoteNumber());
      }
      mMidiQueue.Remove();
    }
    double allLeft = 0.0;
    double allRight = 0.0;
    for (int i = 0; i < kNumVoices; ++i)
    {
      if (mVoices[i] != -1)
      {
        double sound = mVoice[i].getMono();
        allLeft += sound;
        allRight += sound;
        if (mVoice[i].mVolumeEnvelope.inPhase() == Envelope::kIdle)
        {
          mVoices[i] = -1; // Not allocated anymore
        }
      }
    }
    *out01++ = allLeft;
    *out02++ = allRight;
  }

//  mDSP.ProcessBlock(nullptr, outputs, 2, nFrames, mTimeInfo.mPPQPos, mTimeInfo.mTransportIsRunning);
//  mMeterSender.ProcessBlock(outputs, nFrames, kCtrlTagMeter);
//  mLFOVisSender.PushData({kCtrlTagLFOVis, {float(mDSP.mLFO.GetLastOutput())}});
}

void SimpleSynth::OnIdle()
{
  mMeterSender.TransmitData(*this);
  mLFOVisSender.TransmitData(*this);
}

void SimpleSynth::OnReset()
{
  mDSP.Reset(GetSampleRate(), GetBlockSize());
  mMeterSender.Reset(GetSampleRate());

  for (int i = 0; i < kNumVoices; ++i)
  {
    mVoice[i].SetSampleRate(GetSampleRate());
  }
}

void SimpleSynth::ProcessMidiMsg(const IMidiMsg& msg)
{
  TRACE;
  mMidiQueue.Add(msg);  // Take care of MIDI events in ProcessBlock()
}

void
SimpleSynth::OnParamChange(int paramIdx)
{
//  mDSP.SetParam(paramIdx, GetParam(paramIdx)->Value());

  double value = GetParam(paramIdx)->Value();
  switch (paramIdx)
  {
    case kParamDetuneAmount1:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].SetDetuneAmount(kOsc1, value);
      }
      break;
    case kParamDetuneAmount2:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].SetDetuneAmount(kOsc2, value);
      }
      break;
    case kParamMixOscillators:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].SetVolume(kOsc1, value);
        mVoice[i].SetVolume(kOsc2, 1.0 - value);
      }
      break;
    case kParamOsc2Semitone:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].SetOsc2Semitone(static_cast<int>(value));
      }
      break;
    case kParamVolumeAttack:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].mVolumeEnvelope.setAttack(value);
      }
      break;
    case kParamVolumeDecay:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].mVolumeEnvelope.setDecay(value);
      }
      break;
    case kParamVolumeSustain:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].mVolumeEnvelope.setSustain(value);
      }
      break;
    case kParamVolumeRelease:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].mVolumeEnvelope.setRelease(value);
      }
      break;
    case kParamFilterCutoff:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].mFilter.setBaseCutoff(value);
      }
      break;
    case kParamFilterResonance:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].mFilter.setResonance(value);
      }
      break;
    case kParamFilterAttack:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].mFilterEnvelope.setAttack(value);
      }
      break;
    case kParamFilterDecay:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].mFilterEnvelope.setDecay(value);
      }
      break;
    case kParamFilterSustain:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].mFilterEnvelope.setSustain(value);
      }
      break;
    case kParamFilterRelease:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].mFilterEnvelope.setRelease(value);
      }
      break;

    case kParamFilterEnvelopeAmount:
      for (int i = 0; i < kNumVoices; ++i)
      {
        mVoice[i].mFilterEnvelope.setAmount(value);
      }
      break;

      
   

  }
}

void SimpleSynth::OnParamChangeUI(int paramIdx, EParamSource source)
{
  #if IPLUG_EDITOR
  if (auto pGraphics = GetUI())
  {
    if (paramIdx == kParamLFORateMode)
    {
      const auto sync = GetParam(kParamLFORateMode)->Bool();
      pGraphics->HideControl(kParamLFORateHz, sync);
      pGraphics->HideControl(kParamLFORateTempo, !sync);
    }
  }
  #endif
}

bool SimpleSynth::OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData)
{
  if(ctrlTag == kCtrlTagBender && msgTag == IWheelControl::kMessageTagSetPitchBendRange)
  {
    const int bendRange = *static_cast<const int*>(pData);
    mDSP.mSynth.SetPitchBendRange(bendRange);
  }
  
  return false;
}
#endif
