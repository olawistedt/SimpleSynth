#ifndef __VOICE_H__
#define __VOICE_H__

#include "DetunedSawOscillator.h"
#include "Envelope.h"
#include "Filter.h"

const int kOsc1 = 0;
const int kOsc2 = 1;

class Voice
{
public:
  Voice();
  void SetSampleRate(double sampleRate);
  void NoteOn(unsigned char ucNote);
  void NoteOff(unsigned char ucNote);
  void setVolumeEnvelope(Envelope::type parameter, double value);
  double getMono();
  double getLeft();
  double getRight();
  void SetDetuneAmount(int oscNr, double cents);
  void SetVolume(int oscNr, double volume);
  void SetOsc2Semitone(int semitone);

  Envelope mVolumeEnvelope;
  Filter mFilter;

private:
  DetunedSawOscillator m_detunedOscillator1;
  DetunedSawOscillator m_detunedOscillator2;
  double m_note2freq[140];
  int m_osc2semitone;
};

#endif  // __VOICE_H__
