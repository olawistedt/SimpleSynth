#pragma once

#include "SawOscillator.h"
#include <vector>
#include <random>

class DetunedSawOscillator
{
public:
  DetunedSawOscillator();

  void SetSampleRate(double sampleRate);
  void SetBaseFrequency(double freq);
  void SetNumUnisonVoices(int numVoices);
  void SetDetuneAmount(double detuneCents);
  void ResetUnisonPhases();

  double Process();

private:
  std::vector<SawOscillator> m_unisonOscillators;
  int m_numUnisonVoices = 1;
  double m_baseFrequency = 440.0;
  double m_detuneCents = 0.0;
  double m_sampleRate = 44100.0;

  std::mt19937 m_gen;
  std::uniform_real_distribution<> m_detuneDist;

  void UpdateUnisonFrequencies();
  double CentsToFrequencyRatio(double cents);
};