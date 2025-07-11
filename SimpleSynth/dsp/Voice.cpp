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

  m_detunedOscillator1.SetNumUnisonVoices(10);
  m_detunedOscillator2.SetNumUnisonVoices(10);
  mVolumeEnvelope = Envelope();
}

void
Voice::SetSampleRate(double sampleRate)
{
  m_detunedOscillator1.SetSampleRate(sampleRate);
  m_detunedOscillator2.SetSampleRate(sampleRate);
  mVolumeEnvelope.setSampleRate(sampleRate);
}

void
Voice::NoteOn(unsigned char ucNote)
{
  double freq = m_note2freq[ucNote];
  m_detunedOscillator1.SetBaseFrequency(freq);
  m_detunedOscillator1.ResetUnisonPhases();
  m_detunedOscillator2.SetBaseFrequency(m_note2freq[ucNote + m_osc2semitone]);
  m_detunedOscillator2.ResetUnisonPhases();
  mVolumeEnvelope.restart();
}

void
Voice::NoteOff(unsigned char ucNote)
{
  mVolumeEnvelope.beginReleasePhase();
}

double
Voice::getMono()
{
  double sample = m_detunedOscillator1.Process();
  sample += m_detunedOscillator2.Process();
  return sample / 2.0 * mVolumeEnvelope.get();
}

double
Voice::getLeft()
{
  return getMono();
}

double
Voice::getRight()
{
  return getMono();
}

void
Voice::SetDetuneAmount(int oscNr, double cents)
{
  if (oscNr == kOsc1)
  {
    m_detunedOscillator1.SetDetuneAmount(cents);
  }
  else
  {
    m_detunedOscillator2.SetDetuneAmount(cents);
  }
}

void
Voice::SetVolume(int oscNr, double volume)
{
  if (oscNr == kOsc1)
  {
    m_detunedOscillator1.SetVolume(volume);
  }
  else
  {
    m_detunedOscillator2.SetVolume(volume);
  }
}

void
Voice::SetOsc2Semitone(int semitone)
{
  m_osc2semitone = semitone;
}
