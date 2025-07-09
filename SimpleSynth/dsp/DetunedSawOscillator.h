#pragma once

#include "SawOscillator.h"
#include <vector>
#include <random>  // För slumpmässig detuning
#include <array>   // För att returnera två sample (left/right)

class DetunedSawOscillator
{
public:
  DetunedSawOscillator();

  void SetSampleRate(double sampleRate);
  void SetBaseFrequency(double freq);
  void SetNumUnisonVoices(int numVoices);    // Antal detunade vågformar
  void SetDetuneAmount(double detuneCents);  // Max detuning i Cents
  void ResetUnisonPhases();

  // Ändrad: Nu returnerar den ett array med [left_sample, right_sample]
  std::array<double, 2> Process();

private:
  std::vector<SawOscillator> m_unisonOscillators;
  int m_numUnisonVoices = 1;
  double m_baseFrequency = 440.0;
  double m_detuneCents = 0.0;
  double m_sampleRate = 44100.0;

  // För panning: En vektor för att lagra pan-positionen för varje röst (-1.0 till 1.0)
  std::vector<double> m_voicePanPositions;

  std::mt19937 m_gen;
  std::uniform_real_distribution<>
      m_detuneDist;  // Inte längre direkt för detuning, men kan användas för slumpmässig pan om du vill

  void UpdateUnisonFrequencies();
  void UpdateUnisonPans();  // Ny metod för att beräkna pan-positioner
  double CentsToFrequencyRatio(double cents);
};