#pragma once

namespace ProtoRock {
namespace Protocol {
// DeviceOS is a device DeviceOS identifier. It holds a value of one of the constants below and may be found
// in packets such as the Login packet.
enum DeviceOS {
    DeviceAndroid = 1,
    DeviceIOS,
    DeviceOSX,
    DeviceFireOS,
    DeviceGearVR,
    DeviceHololens,
    DeviceWin10,
    DeviceWin32,
    DeviceDedicated,
    DeviceTVOS,
    DeviceOrbis,
    DeviceNX,
    DeviceXBOX,
    DeviceWP,
};
}  // namespace Protocol
}  // namespace ProtoRock