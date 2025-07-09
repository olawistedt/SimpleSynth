#ifndef __VOICE_H__
#define __VOICE_H__

#include "DetunedSawOscillator.h"

class Voice
{
public:
  Voice();
  void NoteOn(unsigned char ucNote);
  void NoteOff(unsigned char ucNote);
  double getStereo();
  void SetDetuneAmount(double cents);

private:
  DetunedSawOscillator m_detunedOscillator;
  double m_note2freq[140];
};

#endif  // __VOICE_H__
