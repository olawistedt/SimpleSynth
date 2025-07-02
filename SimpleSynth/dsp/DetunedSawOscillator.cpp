#include "DetunedSawOscillator.h"
#include <numeric>  // För std::accumulate (om vi vill summera)
#include <cmath>    // För std::pow

DetunedSawOscillator::DetunedSawOscillator() :
  m_gen(std::random_device()()),
  m_detuneDist(-1.0, 1.0)
{
  SetNumUnisonVoices(1);
}

void
DetunedSawOscillator::SetSampleRate(double sampleRate)
{
  m_sampleRate = sampleRate;
  for (auto &osc : m_unisonOscillators)
  {
    osc.SetSampleRate(sampleRate);
  }
  UpdateUnisonFrequencies();
}

void
DetunedSawOscillator::SetBaseFrequency(double freq)
{
  m_baseFrequency = freq;
  UpdateUnisonFrequencies();
}

void
DetunedSawOscillator::SetNumUnisonVoices(int numVoices)
{
  m_numUnisonVoices = std::max(1, numVoices);

  m_unisonOscillators.resize(m_numUnisonVoices);

  for (auto &osc : m_unisonOscillators)
  {
    osc.SetSampleRate(m_sampleRate);
  }
  UpdateUnisonFrequencies();
}

void
DetunedSawOscillator::ResetUnisonPhases()
{
  for (auto &osc : m_unisonOscillators)
  {
    osc.ResetPhase();
  }
}

void
DetunedSawOscillator::SetDetuneAmount(double detuneCents)  // Nu tar den in Cents
{
  m_detuneCents = detuneCents;
  ResetUnisonPhases();
  UpdateUnisonFrequencies();
}

// DetunedSawOscillator.cpp

void
DetunedSawOscillator::UpdateUnisonFrequencies()
{
  if (m_baseFrequency == 0.0)
  {
    for (auto &osc : m_unisonOscillators)
    {
      osc.SetFrequency(0.0);
    }
    return;
  }

  for (int i = 0; i < m_numUnisonVoices; ++i)
  {
    double normalizedOffset;
    if (m_numUnisonVoices == 1)
    {
      normalizedOffset = 0.0;
    }
    else
    {
      // Fördelar rösterna jämnt över intervallet [-1.0, 1.0]
      normalizedOffset = -1.0 + (2.0 * i) / (m_numUnisonVoices - 1.0);
    }

    double centsOffset = normalizedOffset * m_detuneCents;
    double detunedFreq = m_baseFrequency * CentsToFrequencyRatio(centsOffset);
    m_unisonOscillators[i].SetFrequency(detunedFreq);
  }
}

double
DetunedSawOscillator::Process()
{
  double mixedSample = 0.0;
  for (auto &osc : m_unisonOscillators)
  {
    mixedSample += osc.Process();
  }

  return mixedSample / m_numUnisonVoices;  // Normalisera för att undvika klippning
}

// Ny hjälpfunktion för att konvertera cents till en frekvensratio
double
DetunedSawOscillator::CentsToFrequencyRatio(double cents)
{
  return std::pow(2.0, cents / 1200.0);  // Varje 1200 cents är en oktav (faktor 2)
}