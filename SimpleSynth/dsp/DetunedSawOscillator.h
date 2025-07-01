#pragma once

#include "SawOscillator.h"
#include <vector>
#include <random> // För slumpmässig detuning

class DetunedSawOscillator
{
public:
  DetunedSawOscillator();
  
  void SetSampleRate(double sampleRate);
  void SetBaseFrequency(double freq);
  void SetNumUnisonVoices(int numVoices); // Antal detunade vågformar
  void SetDetuneAmount(double detuneHz);  // Max detuning i Hz

  double Process(); // Beräknar nästa sampel från alla detunade vågformar

private:
  std::vector<SawOscillator> m_unisonOscillators;
  int m_numUnisonVoices = 1;
  double m_baseFrequency = 440.0;
  double m_detuneAmount = 0.0;
  double m_sampleRate = 44100.0;

  // För slumpmässig detuning
  std::mt19937 m_gen;
  std::uniform_real_distribution<> m_detuneDist;

  void UpdateUnisonFrequencies(); // Uppdaterar frekvenser för alla unisontoner
};