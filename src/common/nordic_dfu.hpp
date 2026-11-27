// Flashes an application through Nordic's open USB DFU bootloader (nRF52840 Dongle style, as on
// Holyiot dongles): the serial DFU protocol nrfutil and nRF Connect Programmer speak, over the
// bootloader's USB serial port (VID 1915, PID 521F). Unsigned images, no SoftDevice.
#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace lh::dfu {

struct Image {
  uint32_t base = 0;           // flash address of data[0]
  std::vector<uint8_t> data;   // gaps filled with 0xFF, padded to whole words
};

// An Intel HEX file. Fails on anything outside [minAddress, maxAddress).
bool LoadHex(const std::string& path, uint32_t minAddress, uint32_t maxAddress, Image* image, std::string* error);

// Serial ports of plugged-in Nordic bootloaders: "COM7" on Windows, "/dev/ttyACM0" on Linux.
std::vector<std::string> FindBootloaderPorts();

// Sends the image; the bootloader checks it and starts it. progress: 0..1. initPacket: a signed one
// (Valve's bootloader); none: the unsigned packet Nordic's open bootloader takes.
bool Flash(const std::string& port, const Image& image, const std::function<void(float)>& progress, std::string* error,
           const std::vector<uint8_t>* initPacket = nullptr);

// ---- Valve's radio bootloader (Index HMD radio, 28DE:2104): nRF5 SDK 15.2 secure serial DFU ----
struct ValvePort {
  std::string port, serial;   // serial: the radio's USB serial, the same in its application (28DE:2102)
};
std::vector<ValvePort> FindValveBootloaders();
// What the bootloader holds: the application's version and where the application starts.
bool ValveBootloaderInfo(const std::string& port, uint32_t* version, uint32_t* appAddress, std::string* error);
// Leaves the bootloader for the application.
void ValveBootloaderExit(const std::string& port);
// An init packet for `app` from a signed reference (the .dat of a Valve radio package): its hardware
// version, type and SoftDevice requirement with this image's size and hash, and `version`.
// hwVersion: the reference's hardware version.
bool ValveInitPacket(const std::vector<uint8_t>& referenceDat, uint32_t version, const std::vector<uint8_t>& app,
                     std::vector<uint8_t>* packet, uint32_t* hwVersion, std::string* error);
// One file out of a zip (stored or deflated).
bool ReadZipEntry(const std::string& zip, const std::string& name, std::vector<uint8_t>* out);

// ---- SlimeVR nRF Receiver (1209:7690) CDC ACM console ----
struct SlimeVrPort {
  std::string port;
  std::string board;   // "Board: ..." from `info`
  std::string target;  // "Target: ..." from `info`
};
std::vector<SlimeVrPort> FindSlimeVrReceivers();
bool SendSlimeVrDfu(const std::string& port, std::string* error);

}  // namespace lh::dfu
