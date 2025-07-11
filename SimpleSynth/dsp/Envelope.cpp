///////////////////////////////////////////////////////////////////////////////
//
//
//
//                /:\
//               / :  \
//              /  :    \
//             /   :      \
//            /    :        \_____________..._____
//           /     :         :                    :\
//          /      :         :      |             :  \
//         /       :         :      S             :    \
//        /        :         :      |             :      \
//        <---A---> <---D--->                      <--R-->
//
//
///////////////////////////////////////////////////////////////////////////////
#include "Envelope.h"

Envelope::Envelope() : m_fAmount(1.0)
{
  m_ucInPhase = kIdle;

  m_fAttack = 100;    // Msek
  m_fDecay = 100;     // Msek
  m_fSustain = 0.5;
  m_fRelease = 1000;  // Msek
}
void
Envelope::restart()
{
  m_ulCurrent = 0;
  m_ucInPhase = kAttackPhase;
}
void
Envelope::beginReleasePhase()
{
  m_dReleaseStartLevel = get();
  m_ulCurrent = 0;
  m_ucInPhase = kReleasePhase;
}
void
Envelope::setSampleRate(double sampleRate)
{
  m_sampleRate = sampleRate;
  m_fMsecsBetweenSamples =
      1000.0 / m_sampleRate;  // 1 sampleFrame in 96kb/sek = 0.0104 millisec
  setAttack(m_fAttack);
  setDecay(m_fDecay);
  setRelease(m_fRelease);
}
void
Envelope::setAttack(double dMsec)
{
  m_fAttack = dMsec;
  m_ulAttackEnds = (long)(m_fAttack / m_fMsecsBetweenSamples);
}
void
Envelope::setDecay(double dMsec)
{
  m_fDecay = dMsec;
  m_ulDecayEnds = (long)(m_fDecay / m_fMsecsBetweenSamples);
}
void
Envelope::setSustain(double dLevel)
{
  m_fSustain = dLevel;
}
void
Envelope::setRelease(double fMsec)
{
  m_fRelease = fMsec;
  m_ulReleaseEnds = (long)(m_fRelease / m_fMsecsBetweenSamples);
}
void
Envelope::setAmount(double dLevel)
{
  m_fAmount = dLevel;
}
double
Envelope::getAmount()
{
  return m_fAmount;
}

double
Envelope::get()
{
  //  static bool ba,bd,bs,br = false;

  double dRet = 1.0;

  static bool isBypassEnvelope = false;
  if (isBypassEnvelope)
  {
    if (m_ucInPhase == kReleasePhase)
    {
      return 0.0;
    }
    return 1.0;
  }

  if (m_ucInPhase == kAttackPhase)
  {
    if (m_ulCurrent == m_ulAttackEnds)
    {
      m_ucInPhase = kDecayPhase;
      m_ulCurrent = 0;
    }
    else
    {
      dRet = ((double)m_ulCurrent) * m_fMsecsBetweenSamples * 1.0f / m_fAttack;
      m_ulCurrent++;
    }
  }
  if (m_ucInPhase == kDecayPhase)
  {
    if (m_ulCurrent == m_ulDecayEnds)
    {
      m_ucInPhase = kSustainPhase;
      m_ulCurrent = 0;
    }
    else
    {
      dRet = 1.0f - ((double)m_ulCurrent) * m_fMsecsBetweenSamples * (1.0f - m_fSustain) / m_fDecay;
      m_ulCurrent++;
    }
  }
  if (m_ucInPhase == kSustainPhase)
  {
    dRet = m_fSustain;
  }
  if (m_ucInPhase == kReleasePhase)
  {
    if (m_ulCurrent == m_ulReleaseEnds)
    {
      m_ucInPhase = kIdle;
      m_ulCurrent = 0;
    }
    else
    {
      dRet = m_dReleaseStartLevel -
             ((double)m_ulCurrent) * m_fMsecsBetweenSamples * m_dReleaseStartLevel / m_fRelease;
      m_ulCurrent++;
    }
  }
  if (m_ucInPhase == kIdle)
  {
    dRet = 0.0;
  }
  return dRet;
}
