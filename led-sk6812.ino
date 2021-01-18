#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

const uint16_t PIXEL_COUNT = 300; // make sure to set this to the number of pixels in your strip
const uint8_t PIXEL_PIN = 2;  // make sure to set this to the correct pin, ignored for Esp8266
RgbColor startEyeColor = RgbColor(0x7f, 0, 0);
RgbColor endEyeColor = RgbColor(0, 0, 0x7f);
RgbColor currentColor = startEyeColor;

NeoPixelBus<NeoGrbwFeature, NeoSk6812Method> strip(PIXEL_COUNT, PIXEL_PIN);
// for esp8266 omit the pin
//NeoPixelBus<NeoGrbwFeature, NeoSk6812Method> strip(PIXEL_COUNT);

NeoPixelAnimator animations(3);

uint16_t lastPixel = 0; // track the eye position
int8_t moveDir = 1; // track the direction of movement

// uncomment one of the lines below to see the effects of
// changing the ease function on the movement animation
AnimEaseFunction moveEase =
//      NeoEase::Linear;
//      NeoEase::QuadraticInOut;
//      NeoEase::CubicInOut;
//        NeoEase::QuarticInOut;
//      NeoEase::QuinticInOut;
//      NeoEase::SinusoidalInOut;
      NeoEase::ExponentialInOut;
//      NeoEase::CircularInOut;

void FadeAll(uint8_t darkenBy)
{
    RgbwColor color;
    for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++)
    {
        color = strip.GetPixelColor(indexPixel);
        color.Darken(darkenBy);
        strip.SetPixelColor(indexPixel, color);
    }
}

void FadeAnimUpdate(const AnimationParam& param)
{
    if (param.state == AnimationState_Completed)
    {
        FadeAll(10);
        animations.RestartAnimation(param.index);
    }
}

void MoveAnimUpdate(const AnimationParam& param)
{
    float progress = moveEase(param.progress);
    uint16_t nextPixel;
    if (moveDir > 0) {
        nextPixel = progress * PIXEL_COUNT;
    } else {
        nextPixel = (1.0f - progress) * PIXEL_COUNT;
    }

    // if progress moves fast enough, we may move more than
    // one pixel, so we update all between the calculated and
    // the last
    if (lastPixel != nextPixel) {
        for (uint16_t i = lastPixel + moveDir; i != nextPixel; i += moveDir) {
            strip.SetPixelColor(i, currentColor);
        }
    }
    strip.SetPixelColor(nextPixel, currentColor);

    lastPixel = nextPixel;

    if (param.state == AnimationState_Completed) {
        // reverse direction of movement
        moveDir *= -1;

        // done, time to restart this position tracking animation/timer
        animations.RestartAnimation(param.index);
    }
}

bool forwardDirBlend = true;

void BlendColorUpdate(const AnimationParam& param) {
    if (param.state == AnimationState_Completed) {
      animations.RestartAnimation(param.index);
      forwardDirBlend = !forwardDirBlend;
    }

    float progress = NeoEase::CubicInOut(param.progress);
    if (forwardDirBlend) {
      currentColor = RgbColor::LinearBlend(startEyeColor, endEyeColor, progress);
    } else {
      currentColor = RgbColor::LinearBlend(endEyeColor, startEyeColor, progress);
    }
}

void SetupAnimations()
{
    // fade all pixels providing a tail that is longer the faster
    // the pixel moves.
    animations.StartAnimation(0, 7, FadeAnimUpdate);

    // take several seconds to move eye fron one side to the other
    animations.StartAnimation(1, 3000, MoveAnimUpdate);
    // and linear blending of the color over the duration
    animations.StartAnimation(2, 3000, BlendColorUpdate);
}

void setup()
{
    strip.Begin();
    strip.Show();

    SetupAnimations();
}

void loop()
{
    animations.UpdateAnimations();
    strip.Show();
}
