#include "Effects.h"
#include <algorithm>  // For std::clamp, std::fill

static const double kMaxDelayMs = 2000.0;

Effects::Effects()
{
  setSampleRate(44100.0);
}

void
Effects::setSampleRate(double sampleRate)
{
  mSampleRate = sampleRate;
  mBufferSize = static_cast<int>(kMaxDelayMs / 1000.0 * mSampleRate) + 1;
  mBufferL.assign(mBufferSize, 0.0);
  mBufferR.assign(mBufferSize, 0.0);
  mWritePos = 0;
  updateDelaySamples();
}

void
Effects::reset()
{
  std::fill(mBufferL.begin(), mBufferL.end(), 0.0);
  std::fill(mBufferR.begin(), mBufferR.end(), 0.0);
  mWritePos = 0;
}

void
Effects::updateDelaySamples()
{
  int s = static_cast<int>(mDelayMs / 1000.0 * mSampleRate);
  mDelaySamples = std::clamp(s, 1, mBufferSize > 1 ? mBufferSize - 1 : 1);
}

void
Effects::setDelayTime(double ms)
{
  mDelayMs = ms;
  updateDelaySamples();
}

void
Effects::setFeedback(double feedback01)
{
  // Clamp below 1.0 so the feedback loop can never run away.
  mFeedback = std::clamp(feedback01, 0.0, 0.95);
}

void
Effects::setMix(double mix01)
{
  mMix = std::clamp(mix01, 0.0, 1.0);
}

void
Effects::processStereo(double& left, double& right)
{
  if (mBufferSize <= 0)
    return;

  int readPos = mWritePos - mDelaySamples;
  if (readPos < 0)
    readPos += mBufferSize;

  double delayedL = mBufferL[readPos];
  double delayedR = mBufferR[readPos];

  // Write the input plus the fed-back delayed signal into the buffer.
  mBufferL[mWritePos] = left + delayedL * mFeedback;
  mBufferR[mWritePos] = right + delayedR * mFeedback;

  // Blend dry and wet.
  left = left * (1.0 - mMix) + delayedL * mMix;
  right = right * (1.0 - mMix) + delayedR * mMix;

  if (++mWritePos >= mBufferSize)
    mWritePos = 0;
}
