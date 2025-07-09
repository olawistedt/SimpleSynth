#include "Voice.h"

Voice::Voice() //: m_ucNote(0)
{
  // ------------- Build note to frequence table ------------------
  // Convert note to frequence.
  double k = 1.059463094359;     // 12th root of 2
  double a = 6.875;              // a
  a *= k;                        // b
  a *= k;                        // bb
  a *= k;                        // c, frequency of midi note 0
  for (int i = 0; i < 140; i++)  // 140 midi notes
  {
    m_note2freq[i] = (double)a;
    a *= k;
  }

  m_detunedOscillator.SetNumUnisonVoices(10);
}

void
Voice::NoteOn(unsigned char ucNote)
{
  double freq = m_note2freq[ucNote];
  m_detunedOscillator.SetBaseFrequency(freq);
}

void
Voice::NoteOff(unsigned char ucNote)
{
}

std::array<double, 2>
Voice::getStereo()
{
  return m_detunedOscillator.Process();
}

void
Voice::SetDetuneAmount(double cents)
{
  m_detunedOscillator.SetDetuneAmount(cents);
}
