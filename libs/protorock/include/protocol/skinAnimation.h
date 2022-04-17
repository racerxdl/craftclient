#pragma once

#include <string>

// See https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/login/data.go#L232
namespace ProtoRock {
namespace Protocol {
// SkinAnimation is an animation that may be present. It is applied on top of the skin default and is cycled
// through client-side.
struct SkinAnimation {
    // Frames is the amount of frames of the animation. The number of Frames here specifies how many
    // frames may be found in the Image data.
    double Frames;
    // Image is a base64 encoded byte slice of ImageWidth * ImageHeight bytes. It is an RGBA ordered byte
    // representation of the animation image pixels. The ImageData contains FrameCount images in it, which
    // each represent one stage of the animation. The actual part of the skin that this field holds
    // depends on the Type, where SkinAnimationHead holds only the head and its hat, whereas the other
    // animations hold the entire body of the skin.
    std::string Image;
    // ImageHeight and ImageWidth are the dimensions of the animated image. Note that the size of this
    // image is not always 32/64/128.
    int ImageHeight, ImageWidth;
    // Type is the type of the animation, which defines what part of the body the Image data holds. It is
    // one of the following:
    // 0 -> 'None', doesn't typically occur.
    // 1 -> Face animation.
    // 2 -> 32x32 Body animation.
    // 3 -> 128x128 Body animation.
    int Type;
    // ExpressionType is the type of expression made by the skin, which is one of the following:
    // 0 -> Linear.
    // 1 -> Blinking.
    int AnimationExpression;
};
}  // namespace Protocol
}  // namespace ProtoRock