#ifndef __VOICE_H__
#define __VOICE_H__

#include "DetunedSawOscillator.h"

const int kOsc1 = 0;
const int kOsc2 = 1;

class Voice
{
public:
  Voice();
  void NoteOn(unsigned char ucNote);
  void NoteOff(unsigned char ucNote);
  double getMono();
  double getLeft();
  double getRight();
  void SetDetuneAmount(int oscNr, double cents);

private:
  DetunedSawOscillator m_detunedOscillator1;
  DetunedSawOscillator m_detunedOscillator2;
  double m_note2freq[140];
};

#endif  // __VOICE_H__
