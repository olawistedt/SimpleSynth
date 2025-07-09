#include "DetunedSawOscillator.h"
#include <numeric>
#include <cmath>
#include <algorithm>  // För std::min/max

DetunedSawOscillator::DetunedSawOscillator() :
  m_gen(std::random_device()()),
  m_detuneDist(
      -1.0,
      1.0)  // Fortfarande här men inte direkt använd för detune, kan användas för slumpmässig pan
{
  SetNumUnisonVoices(1);  // Standard en röst
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
  m_voicePanPositions.resize(m_numUnisonVoices);  // Storleksändra pan-vektorn

  for (auto &osc : m_unisonOscillators)
  {
    osc.SetSampleRate(m_sampleRate);
  }
  UpdateUnisonFrequencies();  // Uppdatera frekvenser
  UpdateUnisonPans();         // Uppdatera pan-positioner när antalet röster ändras
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

// NY METOD: Uppdaterar pan-positionerna för varje röst
void
DetunedSawOscillator::UpdateUnisonPans()
{
  if (m_numUnisonVoices == 1)
  {
    m_voicePanPositions[0] = 0.0;  // Enkel röst är i mitten
  }
  else
  {
    for (int i = 0; i < m_numUnisonVoices; ++i)
    {
      // Sprid rösterna jämnt över stereo fältet.
      // Ex: för 4 röster: -1.0 (L), -0.33 (L), 0.33 (R), 1.0 (R)
      m_voicePanPositions[i] = -1.0 + (2.0 * i) / (m_numUnisonVoices - 1.0);
    }
  }
}

// ÄNDRAD: Nu returnerar den [left_sample, right_sample]
std::array<double, 2>
DetunedSawOscillator::Process()
{
  double mixedLeftSample = 0.0;
  double mixedRightSample = 0.0;

  for (int i = 0; i < m_numUnisonVoices; ++i)
  {
    double currentOscSample = m_unisonOscillators[i].Process();
    double pan = m_voicePanPositions[i];  // Pan-position för denna röst

    // Pan-lag (konstant effekt-pan, "equal power" är mer komplext)
    // Enkel linjär pan:
    double panLeft = std::sqrt(0.5 * (1.0 - pan));   // square root of 0.5 * (1 - pan)
    double panRight = std::sqrt(0.5 * (1.0 + pan));  // square root of 0.5 * (1 + pan)

    mixedLeftSample += currentOscSample * panLeft;
    mixedRightSample += currentOscSample * panRight;
  }

  // Normalisera summan för att undvika klippning
  // En grov normalisering: dela med sqrt av antalet röster.
  // Detta hjälper till att bevara den upplevda volymen.
  double normalisationFactor = 1.0 / std::sqrt(static_cast<double>(m_numUnisonVoices));

  return { mixedLeftSample * normalisationFactor, mixedRightSample * normalisationFactor };
}

double
DetunedSawOscillator::CentsToFrequencyRatio(double cents)
{
  return std::pow(2.0, cents / 1200.0);
}