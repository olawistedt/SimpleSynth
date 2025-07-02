#pragma once

#include "SawOscillator.h"
#include <vector>
#include <random>  // För slumpmässig detuning

class DetunedSawOscillator
{
public:
  DetunedSawOscillator();

  void SetSampleRate(double sampleRate);
  void SetBaseFrequency(double freq);
  void SetNumUnisonVoices(int numVoices);    // Antal detunade vågformar
  void SetDetuneAmount(double detuneCents);  // Max detuning i Cents

  double Process();  // Beräknar nästa sampel från alla detunade vågformar

private:
  std::vector<SawOscillator> m_unisonOscillators;
  int m_numUnisonVoices = 1;
  double m_baseFrequency = 440.0;
  double m_detuneCents = 0.0;  // Ändrad till Cents
  double m_sampleRate = 44100.0;

  // För slumpmässig detuning
  std::mt19937 m_gen;
  std::uniform_real_distribution<> m_detuneDist;  // Kommer fortfarande att generera -1.0 till 1.0

  void UpdateUnisonFrequencies();              // Uppdaterar frekvenser för alla unisontoner
  double CentsToFrequencyRatio(double cents);  // Ny hjälpfunktion
};