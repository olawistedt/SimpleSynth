#pragma once

#include "SawOscillator.h"
#include <vector>
#include <random>
#include <array>

class DetunedSawOscillator
{
public:
  DetunedSawOscillator();

  void SetSampleRate(double sampleRate);
  void SetBaseFrequency(double freq);
  void SetDetuneAmount(double detuneCents);
  void ResetUnisonPhases();

  double Process();

private:
  std::vector<SawOscillator> m_unisonOscillators;
  int m_numUnisonVoices = 10;
  double m_baseFrequency = 440.0;
  double m_detuneCents = 0.0;
  double m_sampleRate = 44100.0;

  void UpdateUnisonFrequencies();
  double CentsToFrequencyRatio(double cents);
};