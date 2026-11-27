// SlimeVR / smol nRF receiver PCB catalog and runtime board_cfg_t ("OHBC") image patcher.
// Keeps a single generic Doorman firmware per SoC + bootloader layout (4 base images total)
// and patches the 64-byte board descriptor in-place for any compatible official or community PCB.
#pragma once
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "common/dongle_bank.hpp"

namespace lh::smol {

constexpr uint32_t kBoardCfgMagic = 0x4342484Fu;  // "OHBC"
constexpr uint16_t kBoardCfgVersion = 1u;
constexpr uint8_t kPinNone = 0xFFu;

constexpr uint8_t Pin(uint8_t port, uint8_t num) { return uint8_t((port << 5) | (num & 31u)); }

enum class Soc : uint8_t { Nrf52840, Nrf52833 };
enum class Boot : uint8_t { Uf2NoSd, Uf2Sd, Uf2Auto, NordicDfu };
enum : uint8_t { kFemNone = 0, kFemTwoPin = 1 };
enum : uint8_t { kLedNone = 0, kLedGpioHigh = 1, kLedGpioLow = 2, kLedWs2812Spi = 3 };
enum : uint8_t { kFlagRegout3v0 = 1u << 0, kFlagNordicDfu = 1u << 1 };

struct SmolBoard {
  const char* id;            // CONFIG_BOARD name from SlimeVR `info`
  const char* displayName;   // UI label
  const char* usbProduct;    // USB product string (<= 39 chars, starts with "Doorman ")
  Soc soc;
  Boot boot;
  uint8_t boardId;           // CMD_INFO byte 57 (5 for community smol boards)
  uint8_t femType;
  uint8_t femCtx;
  uint8_t femCrx;
  int8_t txPower;            // dBm
  uint8_t ledType;
  uint8_t ledPin;
  uint8_t ledPin2;
  uint8_t flags;
};

// All supported official + community (jitingcn) SlimeVR nRF52840 / nRF52833 receivers.
// SKY66112, RFX2401C, and AT2401C share the same two-pin CTX/CRX FEM driver (kFemTwoPin).
inline const std::vector<SmolBoard>& SupportedBoards() {
  static const std::vector<SmolBoard> kBoards = {
      // ---- nRF52833 (UF2 0x621E937A, 833_uf2 layout) ----
      {"sk_cheesecake_recv33", "SK Cheesecake Receiver 33 (nRF52833)", "Doorman Dongle SK Cheesecake RECV33",
       Soc::Nrf52833, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(0, 17), Pin(0, 15), -4, kLedWs2812Spi, Pin(1, 9), Pin(0, 2), kFlagRegout3v0},
      {"foxdongle33_uf2", "FoxDongle33 (nRF52833)", "Doorman Dongle FoxDongle33",
       Soc::Nrf52833, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(0, 17), Pin(0, 15), -4, kLedGpioHigh, Pin(0, 28), kPinNone, kFlagRegout3v0},
      {"nekonya_uf2", "NekoNya Receiver (nRF52833)", "Doorman Dongle NekoNya",
       Soc::Nrf52833, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(0, 17), Pin(0, 15), -4, kLedGpioLow, Pin(0, 28), kPinNone, kFlagRegout3v0},
      {"dawnrx_2_uf2", "Dawn Tracker Receiver 2 (nRF52833)", "Doorman Dongle DawnRX 2",
       Soc::Nrf52833, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(0, 10), Pin(0, 9), -4, kLedGpioLow, Pin(1, 9), kPinNone, kFlagRegout3v0},
      {"butterfly_p2_uf2", "SlimeVR Butterfly P2 (nRF52833)", "Doorman Dongle Butterfly P2",
       Soc::Nrf52833, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(0, 10), Pin(0, 9), -4, kLedNone, kPinNone, kPinNone, kFlagRegout3v0},
      {"butterfly_p5_uf2", "SlimeVR Butterfly P5 (nRF52833)", "Doorman Dongle Butterfly P5",
       Soc::Nrf52833, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(0, 10), Pin(0, 9), -4, kLedNone, kPinNone, kPinNone, kFlagRegout3v0},
      {"butterfly_dg1_uf2", "SlimeVR Butterfly DG1 (nRF52833)", "Doorman Dongle Butterfly DG1",
       Soc::Nrf52833, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(0, 10), Pin(0, 9), -4, kLedNone, kPinNone, kPinNone, kFlagRegout3v0},

      // ---- nRF52840 UF2 (0xADA52840) ----
      {"aerorx_r1_uf2", "AeroRX R1 (nRF52840, UF2)", "Doorman Dongle AeroRX R1",
       Soc::Nrf52840, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(0, 24), Pin(0, 22), -4, kLedGpioLow, Pin(0, 13), kPinNone, kFlagRegout3v0},
      {"estenova_r1_uf2", "Estenova R1 (nRF52840, UF2)", "Doorman Dongle Estenova R1",
       Soc::Nrf52840, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(0, 24), Pin(0, 22), -4, kLedGpioLow, Pin(0, 8), kPinNone, kFlagRegout3v0},
      {"styria_r1_uf2", "Styria R1 (nRF52840, UF2)", "Doorman Dongle Styria R1",
       Soc::Nrf52840, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(0, 24), Pin(0, 22), -4, kLedGpioLow, Pin(0, 6), kPinNone, kFlagRegout3v0},
      {"foxdongle_uf2", "FoxDongle (nRF52840, UF2)", "Doorman Dongle FoxDongle",
       Soc::Nrf52840, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(1, 0), Pin(0, 24), -4, kLedGpioLow, Pin(1, 9), kPinNone, kFlagRegout3v0},
      {"foxdongleneo_uf2", "FoxDongleNEO (nRF52840, UF2)", "Doorman Dongle FoxDongleNEO",
       Soc::Nrf52840, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(0, 10), Pin(0, 9), -4, kLedGpioHigh, Pin(0, 13), kPinNone, kFlagRegout3v0},
      {"foxpromicro_rfek_uf2", "FoxProMicro RFEK (nRF52840, UF2)", "Doorman Dongle FoxProMicro RFEK",
       Soc::Nrf52840, Boot::Uf2NoSd, 5, kFemTwoPin, Pin(1, 0), Pin(0, 11), -4, kLedGpioHigh, Pin(0, 15), kPinNone, kFlagRegout3v0},
      {"promicro_uf2", "ProMicro / nice!nano (nRF52840, UF2)", "Doorman Dongle ProMicro",
       Soc::Nrf52840, Boot::Uf2Auto, 5, kFemNone, kPinNone, kPinNone, 8, kLedGpioHigh, Pin(0, 15), kPinNone, 0},
      {"xiao_ble", "Seeed XIAO nRF52840 (UF2 SD)", "Doorman Dongle XIAO BLE",
       Soc::Nrf52840, Boot::Uf2Sd, 5, kFemNone, kPinNone, kPinNone, 8, kLedNone, kPinNone, kPinNone, 0},
      {"etee_dongle_uf2", "etee Dongle (nRF52840, UF2)", "Doorman Dongle etee",
       Soc::Nrf52840, Boot::Uf2NoSd, 5, kFemNone, kPinNone, kPinNone, 8, kLedGpioLow, Pin(0, 7), kPinNone, 0},

      // ---- nRF52840 Nordic Open USB DFU ----
      {"holyiot_21017", "Holyiot 21017 (nRF52840, Nordic DFU)", "Doorman Dongle Holyiot 21017",
       Soc::Nrf52840, Boot::NordicDfu, 5, kFemTwoPin, Pin(0, 24), Pin(0, 22), -4, kLedGpioLow, Pin(0, 8), kPinNone, kFlagNordicDfu},
      {"nrf52840dongle", "Nordic PCA10059 / eByte (nRF52840, Nordic DFU)", "Doorman Dongle PCA10059",
       Soc::Nrf52840, Boot::NordicDfu, 5, kFemNone, kPinNone, kPinNone, 8, kLedGpioLow, Pin(0, 6), kPinNone, kFlagNordicDfu | kFlagRegout3v0},
  };
  return kBoards;
}

inline std::string Lower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return char(std::tolower(c)); });
  return s;
}

// Returns non-empty reason if the board reported by SlimeVR `info` is explicitly unsupported.
inline std::string UnsupportedReason(const std::string& board, const std::string& target) {
  const std::string b = Lower(board), t = Lower(target);
  if (b.find("butterfly_p3") != std::string::npos || b.find("kounolab") != std::string::npos ||
      t.find("nrf52820") != std::string::npos) {
    return "nRF52820 (32 KB RAM) cannot run Doorman firmware (requires >= 128 KB RAM)";
  }
  if (b.find("butterfly_p1") != std::string::npos || b.find("nrf21540") != std::string::npos) {
    return "nRF21540 4-pin FEM is not yet supported in this build";
  }
  return {};
}

// Matches SlimeVR `info` Board/Target string to an index in SupportedBoards(), or -1 if unknown.
inline int MatchBoard(const std::string& board, const std::string& target) {
  const std::string b = Lower(board), t = Lower(target);
  const auto& list = SupportedBoards();
  for (size_t i = 0; i < list.size(); ++i) {
    const std::string id = Lower(list[i].id);
    if (b == id || b.find(id) != std::string::npos || t.find(id) != std::string::npos) return int(i);
  }
  return -1;
}

// Base firmware filename under firmwares/dongle/ for a given board (and detected UF2 SoftDevice flag).
inline const char* BaseFirmwareFilename(const SmolBoard& b, bool uf2HasSoftDevice) {
  if (b.soc == Soc::Nrf52833) return "doorman-smol-52833.uf2";
  if (b.boot == Boot::NordicDfu) return "doorman-smol-52840-dfu.hex";
  if (b.boot == Boot::Uf2Sd) return "doorman-smol-52840-sd.uf2";
  if (b.boot == Boot::Uf2Auto && uf2HasSoftDevice) return "doorman-smol-52840-sd.uf2";
  return "doorman-smol-52840-nosd.uf2";
}

inline std::array<uint8_t, 64> PackBoardCfg(const SmolBoard& b) {
  std::array<uint8_t, 64> out{};
  auto put32 = [&](size_t off, uint32_t v) {
    out[off] = uint8_t(v);
    out[off + 1] = uint8_t(v >> 8);
    out[off + 2] = uint8_t(v >> 16);
    out[off + 3] = uint8_t(v >> 24);
  };
  put32(0, kBoardCfgMagic);
  out[4] = uint8_t(kBoardCfgVersion);
  out[5] = uint8_t(kBoardCfgVersion >> 8);
  out[6] = b.boardId;
  out[7] = b.femType;
  out[8] = b.femCtx;
  out[9] = b.femCrx;
  out[10] = kPinNone;
  out[11] = kPinNone;
  out[12] = uint8_t(b.txPower);
  out[13] = b.ledType;
  out[14] = b.ledPin;
  out[15] = b.ledPin2;
  out[16] = b.flags;
  // bytes 17..19: reserved = 0
  const size_t nlen = std::min<size_t>(std::strlen(b.usbProduct), 39);
  std::memcpy(out.data() + 20, b.usbProduct, nlen);
  const std::vector<uint8_t> body(out.begin(), out.begin() + 60);
  put32(60, lh::dongle_bank::Crc32(body));
  return out;
}

inline int FindCfgOffset(const uint8_t* data, size_t size) {
  int found = -1;
  for (size_t i = 0; i + 64 <= size; ++i) {
    if (data[i] == 0x4F && data[i + 1] == 0x48 && data[i + 2] == 0x42 && data[i + 3] == 0x43 &&
        data[i + 4] == uint8_t(kBoardCfgVersion) && data[i + 5] == uint8_t(kBoardCfgVersion >> 8) &&
        data[i + 17] == 0 && data[i + 18] == 0 && data[i + 19] == 0) {
      if (found >= 0) return -2;  // ambiguous
      found = int(i);
    }
  }
  return found;
}

// Patches the unique 64-byte OHBC descriptor inside a raw binary or loaded HEX image.
inline bool PatchRawImage(std::vector<uint8_t>& image, const SmolBoard& b, std::string* error) {
  const int off = FindCfgOffset(image.data(), image.size());
  if (off < 0) {
    if (error) *error = off == -2 ? "ambiguous board config marker in firmware" : "board config marker not found in firmware";
    return false;
  }
  const auto cfg = PackBoardCfg(b);
  std::memcpy(image.data() + off, cfg.data(), cfg.size());
  return true;
}

// Patches the unique 64-byte OHBC descriptor across the 256-byte payloads of a UF2 file.
inline bool PatchUf2Image(std::string& uf2, const SmolBoard& b, std::string* error) {
  if (uf2.size() < 512 || uf2.size() % 512 != 0) {
    if (error) *error = "invalid UF2 image size";
    return false;
  }
  const size_t blocks = uf2.size() / 512;
  std::vector<uint8_t> payload;
  payload.reserve(blocks * 256);
  std::vector<size_t> blockOfByte;
  std::vector<size_t> offsetInBlock;
  for (size_t bi = 0; bi < blocks; ++bi) {
    const uint8_t* blk = reinterpret_cast<const uint8_t*>(uf2.data() + bi * 512);
    uint32_t m0 = 0, m1 = 0, plen = 0;
    std::memcpy(&m0, blk, 4);
    std::memcpy(&m1, blk + 4, 4);
    std::memcpy(&plen, blk + 16, 4);
    if (m0 != 0x0A324655u || m1 != 0x9E5D5157u || plen > 476) {
      if (error) *error = "corrupted UF2 block header";
      return false;
    }
    for (size_t k = 0; k < plen; ++k) {
      payload.push_back(blk[32 + k]);
      blockOfByte.push_back(bi);
      offsetInBlock.push_back(32 + k);
    }
  }
  const int off = FindCfgOffset(payload.data(), payload.size());
  if (off < 0) {
    if (error) *error = off == -2 ? "ambiguous board config marker in UF2" : "board config marker not found in UF2";
    return false;
  }
  const auto cfg = PackBoardCfg(b);
  for (size_t k = 0; k < 64; ++k) {
    const size_t idx = size_t(off) + k;
    uf2[blockOfByte[idx] * 512 + offsetInBlock[idx]] = char(cfg[k]);
  }
  return true;
}

}  // namespace lh::smol
