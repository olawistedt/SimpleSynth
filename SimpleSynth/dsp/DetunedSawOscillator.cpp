#include "DetunedSawOscillator.h"
#include <numeric>
#include <cmath>
#include <algorithm>  // For std::min/max

DetunedSawOscillator::DetunedSawOscillator() : m_numUnisonVoices(10)
{
  m_unisonOscillators.resize(m_numUnisonVoices);
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
DetunedSawOscillator::SetDetuneAmount(double detuneCents)
{
  m_detuneCents = detuneCents;
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
DetunedSawOscillator::UpdateUnisonFrequencies()
{
  for (auto &osc : m_unisonOscillators)
  {
    osc.SetFrequency(m_baseFrequency);
  }
}

double
DetunedSawOscillator::Process()
{
  double currentOscSample = 0.0;
  for (int i = 0; i < m_numUnisonVoices; ++i)
  {
    currentOscSample += m_unisonOscillators[i].Process();
  }

  return currentOscSample / m_numUnisonVoices;
}

double
DetunedSawOscillator::CentsToFrequencyRatio(double cents)
{
  return std::pow(2.0, cents / 1200.0);
}