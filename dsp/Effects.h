#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include <vector>

// Master effects processor. Currently hosts a stereo feedback delay.
class Effects
{
public:
  Effects();

  void setSampleRate(double sampleRate);
  void reset();

  void setDelayTime(double ms);
  void setFeedback(double feedback01);  // 0..1
  void setMix(double mix01);            // 0 = dry, 1 = wet

  // Processes one stereo sample in place.
  void processStereo(double& left, double& right);

private:
  void updateDelaySamples();

  double mSampleRate = 44100.0;
  double mDelayMs = 300.0;
  double mFeedback = 0.35;
  double mMix = 0.25;

  std::vector<double> mBufferL;
  std::vector<double> mBufferR;
  int mBufferSize = 0;
  int mWritePos = 0;
  int mDelaySamples = 1;
};

#endif  // __EFFECTS_H__
