#include "spc1000.h"
#include "cassette.h"
#include "dos.h"
#include "fabutils.h"
#include "sysdep.h"
#include "osd.h"
#include "nvs_flash.h"
#include "nvs.h"

#define I_PERIOD 4000
#define INTR_PERIOD 16.666
#define KBD_PERIOD 50

extern byte mem[];
extern SPC1000 spc;
extern byte mc6847_font[];

extern "C" {

void WrZ80(register uint16 Addr,register byte Value) {
  spc.WriteMem(Addr, Value);
}

byte RdZ80(register uint16 Addr) {
  return spc.ReadMem(Addr);
}

void OutZ80(register uint16 Port,register byte Value) {
  spc.WriteIO(Port, Value);
}

byte InZ80(register uint16 Port) {
  return spc.ReadIO(Port);
}

void CheckHook(register Z80 *R) {}
void PatchZ80(register Z80 *R) {}
uint16 LoopZ80(register Z80 *R) { return INT_NONE; }

uint32_t cas_start_time() {
  return spc.cas_start_time();
}

MC6847 *vdg() {
  return spc.vdg();
}
}  // extern "C"

SPC1000::SPC1000() {}
SPC1000::~SPC1000() {}

void SPC1000::Init() {
  InitMem();

  InitCassette(&cas_);
  cas_.motor = 0;
  cas_.pulse = 0;

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES
      || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition may have been truncated and needs erasing.
    // Retry nvs_flash_init.
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  nvs_init_ = (err == ESP_OK);

  mc6847_.Init(io_);

  mc6847_.SetFontFace(ReadConfig("font_face", 1) ? mc6847_font : &mem_[0x524a]);
  mc6847_.SetCRTEffect(ReadConfig("vintage_crt", 1));

  osd_init();
  ay38910_.Init(&sound_generator_);
  keyboard_.begin(PS2Preset::KeyboardPort0);

  ResetZ80(&cpu_);
  cpu_.ICount = I_PERIOD;
  kbd_timer_ = KBD_PERIOD;
  tick_ = 0;
  iplk_ = 1;

  for (int i = fabgl::VK_NONE; i < fabgl::VK_LAST; ++i) {
    key_table_[i] = { -1, 0 };
  }
  key_table_[fabgl::VK_RSHIFT] = { 0, 0x02 };
  key_table_[fabgl::VK_LSHIFT] = { 0, 0x02 };
  key_table_[fabgl::VK_LCTRL] = { 0, 0x04 };
  key_table_[fabgl::VK_RCTRL] = { 0, 0x04 };
  key_table_[fabgl::VK_PRINTSCREEN] = { 0, 0x10 };
  key_table_[fabgl::VK_LALT] = { 0, 0x40 };
  key_table_[fabgl::VK_RALT] = { 0, 0x40 };

  key_table_[fabgl::VK_GRAVEACCENT] = { 1, 0x01 };
  key_table_[fabgl::VK_HOME] = { 1, 0x02 };
  key_table_[fabgl::VK_SPACE] = { 1, 0x04 };
  key_table_[fabgl::VK_RETURN] = { 1, 0x08 };
  key_table_[fabgl::VK_C] = { 1, 0x10 };
  key_table_[fabgl::VK_c] = { 1, 0x10 };
  key_table_[fabgl::VK_A] = { 1, 0x20 };
  key_table_[fabgl::VK_a] = { 1, 0x20 };
  key_table_[fabgl::VK_q] = { 1, 0x40 };
  key_table_[fabgl::VK_Q] = { 1, 0x40 };
  key_table_[fabgl::VK_EXCLAIM] = { 1, 0x80 };
  key_table_[fabgl::VK_1] = { 1, 0x80 };

  key_table_[fabgl::VK_TAB] = { 2, 0x01 };
  key_table_[fabgl::VK_Z] = { 2, 0x04 };
  key_table_[fabgl::VK_z] = { 2, 0x04 };
  key_table_[fabgl::VK_RIGHTBRACE] = { 1, 0x08 };
  key_table_[fabgl::VK_RIGHTBRACKET] = { 1, 0x08 };
  key_table_[fabgl::VK_V] = { 2, 0x10 };
  key_table_[fabgl::VK_v] = { 2, 0x10 };
  key_table_[fabgl::VK_S] = { 2, 0x20 };
  key_table_[fabgl::VK_s] = { 2, 0x20 };
  key_table_[fabgl::VK_W] = { 2, 0x40 };
  key_table_[fabgl::VK_w] = { 2, 0x40 };
  key_table_[fabgl::VK_AT] = { 2, 0x80 };
  key_table_[fabgl::VK_2] = { 2, 0x80 };

  key_table_[fabgl::VK_BACKSPACE] = { 3, 0x01 };
  key_table_[fabgl::VK_ESCAPE] = { 3, 0x04 };
  key_table_[fabgl::VK_LEFTBRACE] = { 3, 0x08 };
  key_table_[fabgl::VK_LEFTBRACKET] = { 3, 0x08 };
  key_table_[fabgl::VK_B] = { 3, 0x10 };
  key_table_[fabgl::VK_b] = { 3, 0x10 };
  key_table_[fabgl::VK_D] = { 3, 0x20 };
  key_table_[fabgl::VK_d] = { 3, 0x20 };
  key_table_[fabgl::VK_E] = { 3, 0x40 };
  key_table_[fabgl::VK_e] = { 3, 0x40 };
  key_table_[fabgl::VK_HASH] = { 3, 0x80 };
  key_table_[fabgl::VK_3] = { 3, 0x80 };

  key_table_[fabgl::VK_RIGHT] = { 4, 0x04 };
  key_table_[fabgl::VK_BACKSLASH] = { 4, 0x08 };
  key_table_[fabgl::VK_N] = { 4, 0x10 };
  key_table_[fabgl::VK_n] = { 4, 0x10 };
  key_table_[fabgl::VK_F] = { 4, 0x20 };
  key_table_[fabgl::VK_f] = { 4, 0x20 };
  key_table_[fabgl::VK_R] = { 4, 0x40 };
  key_table_[fabgl::VK_r] = { 4, 0x40 };
  key_table_[fabgl::VK_DOLLAR] = { 4, 0x80 };
  key_table_[fabgl::VK_4] = { 4, 0x80 };

  key_table_[fabgl::VK_F1] = { 5, 0x02 };
  key_table_[fabgl::VK_LEFT] = { 5, 0x04 };
  key_table_[fabgl::VK_M] = { 5, 0x10 };
  key_table_[fabgl::VK_m] = { 5, 0x10 };
  key_table_[fabgl::VK_G] = { 5, 0x20 };
  key_table_[fabgl::VK_g] = { 5, 0x20 };
  key_table_[fabgl::VK_T] = { 5, 0x40 };
  key_table_[fabgl::VK_t] = { 5, 0x40 };
  key_table_[fabgl::VK_PERCENT] = { 5, 0x80 };
  key_table_[fabgl::VK_5] = { 5, 0x80 };

  key_table_[fabgl::VK_F2] = { 6, 0x02 };
  key_table_[fabgl::VK_PLUS] = { 6, 0x04 };
  key_table_[fabgl::VK_EQUALS] = { 6, 0x04 };
  key_table_[fabgl::VK_X] = { 6, 0x08 };
  key_table_[fabgl::VK_x] = { 6, 0x08 };
  key_table_[fabgl::VK_LESS] = { 6, 0x10 };
  key_table_[fabgl::VK_COMMA] = { 6, 0x10 };
  key_table_[fabgl::VK_H] = { 6, 0x20 };
  key_table_[fabgl::VK_h] = { 6, 0x20 };
  key_table_[fabgl::VK_Y] = { 6, 0x40 };
  key_table_[fabgl::VK_y] = { 6, 0x40 };
  key_table_[fabgl::VK_CARET] = { 6, 0x80 };
  key_table_[fabgl::VK_6] = { 6, 0x80 };

  key_table_[fabgl::VK_F3] = { 7, 0x02 };
  key_table_[fabgl::VK_UP] = { 7, 0x04 };
  key_table_[fabgl::VK_P] = { 7, 0x08 };
  key_table_[fabgl::VK_p] = { 7, 0x08 };
  key_table_[fabgl::VK_GREATER] = { 7, 0x10 };
  key_table_[fabgl::VK_PERIOD] = { 7, 0x10 };
  key_table_[fabgl::VK_J] = { 7, 0x20 };
  key_table_[fabgl::VK_j] = { 7, 0x20 };
  key_table_[fabgl::VK_U] = { 7, 0x40 };
  key_table_[fabgl::VK_u] = { 7, 0x40 };
  key_table_[fabgl::VK_AMPERSAND] = { 7, 0x80 };
  key_table_[fabgl::VK_7] = { 7, 0x80 };

  key_table_[fabgl::VK_F4] = { 8, 0x02 };
  key_table_[fabgl::VK_DOWN] = { 8, 0x04 };
  key_table_[fabgl::VK_QUOTEDBL] = { 8, 0x08 };
  key_table_[fabgl::VK_QUOTE] = { 8, 0x08 };
  key_table_[fabgl::VK_QUESTION] = { 8, 0x10 };
  key_table_[fabgl::VK_SLASH] = { 8, 0x10 };
  key_table_[fabgl::VK_K] = { 8, 0x20 };
  key_table_[fabgl::VK_k] = { 8, 0x20 };
  key_table_[fabgl::VK_I] = { 8, 0x40 };
  key_table_[fabgl::VK_i] = { 8, 0x40 };
  key_table_[fabgl::VK_ASTERISK] = { 8, 0x80 };
  key_table_[fabgl::VK_8] = { 8, 0x80 };

  key_table_[fabgl::VK_F5] = { 9, 0x02 };
  key_table_[fabgl::VK_MINUS] = { 9, 0x04 };
  key_table_[fabgl::VK_TILDE] = { 9, 0x04 };
  key_table_[fabgl::VK_RIGHTPAREN] = { 9, 0x08 };
  key_table_[fabgl::VK_0] = { 9, 0x08 };
  key_table_[fabgl::VK_COLON] = { 9, 0x10 };
  key_table_[fabgl::VK_SEMICOLON] = { 9, 0x10 };
  key_table_[fabgl::VK_L] = { 9, 0x20 };
  key_table_[fabgl::VK_l] = { 9, 0x20 };
  key_table_[fabgl::VK_O] = { 9, 0x40 };
  key_table_[fabgl::VK_o] = { 9, 0x40 };
  key_table_[fabgl::VK_LEFTPAREN] = { 9, 0x80 };
  key_table_[fabgl::VK_9] = { 9, 0x80 };
}

int SPC1000::ReadConfig(const char* attr, int defvalue) {
  esp_err_t err;
  nvs_handle_t nvs_handle;
  int32_t res;

  if (!nvs_init_) return defvalue;

  err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK) {
    return defvalue;
  }

  err = nvs_get_i32(nvs_handle, attr, &res);
  if (err != ESP_OK) {
    res = defvalue;
  }
  nvs_close(nvs_handle);
  return res;
}

void SPC1000::UpdateConfig(const char *attr, int value) {
  esp_err_t err;
  nvs_handle_t nvs_handle;

  if (!nvs_init_) return;

  err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
  if (err != ESP_OK) return;

  err = nvs_set_i32(nvs_handle, attr, value);
  if (err == ESP_OK) {
    nvs_commit(nvs_handle);
  }
  nvs_close(nvs_handle);
}

void SPC1000::InitMem() {
  mem_ = mem;
  memset(key_matrix_, 0xff, sizeof key_matrix_);
  io_ = (byte *)malloc(0x2000);
  key_table_ = (KeyMat *)malloc(sizeof(KeyMat) * (size_t)fabgl::VK_LAST);

  // Modify KBT for modern keyboard layout
  mem_[0x1311] = 0x27; // 3b '(AT), :(SPC)
  mem_[0x1320] = 0x3d;
  mem_[0x1331] = 0x7c;
  mem_[0x1341] = 0x5d;
  mem_[0x1346] = 0x60;
  mem_[0x1350] = 0x7f;
  mem_[0x1351] = 0x29;
  mem_[0x1352] = 0x3a; // 22 :(AT), +(SPC)
  mem_[0x1355] = 0x28;
  mem_[0x1359] = 0x22; // 3a "(AT), *(SPC)
  mem_[0x135d] = 0x2a;
  mem_[0x1365] = 0x26;
  mem_[0x1368] = 0x2b;
  mem_[0x136d] = 0x5e; // 53
  mem_[0x1379] = 0x5c;
  mem_[0x138d] = 0x40;
  mem_[0x138e] = 0x7e;
}

int SPC1000::ReadMem(int addr) {
  return mem_[iplk_ ? addr & 0x7fff : addr];
}

void SPC1000::WriteIO(int addr, int data) {
  if (0x0000 <= addr && addr < 0x2000) {
    io_[addr] = data;
  } else if ((addr & 0xfffe) == 0x4000) {
    if (addr & 0x1)
      ay38910_.Write(data);
    else
      ay38910_.WrCtrl(data);
  } else if ((addr & 0xe000) == 0x6000) {
    CasIOWrite(&cas_, data);
  } else if ((addr & 0xe000) == 0x2000) {
    mc6847_.SetMode((data & 0x08) == 0, data);
  } else if (addr == 0x800a) {
    mc6847_.SetFontFace(data ? mc6847_font : &mem_[0x524a]);
    UpdateConfig("font_face", data);
  } else if (addr == 0x800b) {
    mc6847_.SetCRTEffect(data);
    UpdateConfig("vintage_crt", data);
  }
}

int SPC1000::ReadIO(int addr) {
  if (0x0000 <= addr && addr < 0x2000) {
    return io_[addr];
  } else if (0x8000 <= addr && addr <= 0x8009) {
    return KeyIOMatrix(addr-0x8000);
  } else if ((addr & 0xfffe) == 0x4000) {
    if (addr & 0x01) {
      if (ay38910_.reg() == 14) {
        return CasIORead(&cas_);
      } else {
        return ay38910_.RdData();
      }
    }
  } else if ((addr & 0xe000) == 0xa000) {
    iplk_ = 0;
  }
  return 0;
}

void SPC1000::Run() {
  double intrTime = INTR_PERIOD;
  simul_.base = get_timestamp_ms();
  simul_.prev = simul_.base;
  tick_ = 0;

  while (true) {
    if (cpu_.ICount <= 0) {
      tick_++;
      cpu_.ICount += I_PERIOD;
      intrTime -= 1.0;
      if (intrTime < 0) {
        intrTime += INTR_PERIOD;
        if (cpu_.IFF & IFF_EI) {
          cpu_.IFF |= IFF_IM1 | IFF_1;
          IntZ80(&cpu_, 0);
        }
      }

      PollKeyboard();

      simul_.cur = get_timestamp_ms() - simul_.base;
      int32_t deltaMs = simul_.cur - simul_.prev;
      if (deltaMs > 0) {
        ay38910_.Loop(deltaMs);
        simul_.prev = simul_.cur;
      }

      while (simul_.cur < tick_) {
        vTaskDelay(1);
        simul_.cur = get_timestamp_ms() - simul_.base;
      }
    }
    ExecZ80(&cpu_);
  }
}

void SPC1000::PollKeyboard() {
  if (--kbd_timer_ > 0) return;

  kbd_timer_ += KBD_PERIOD;
  auto kbd = fabgl::PS2Controller::keyboard();
  if (kbd->virtualKeyAvailable()) {
    VirtualKeyItem item;
    if (kbd->getNextVirtualKey(&item)) {
      KeyMat km = key_table_[static_cast<int>(item.vk)];
      if (km.addr >= 0) {
        if (item.down) {
          key_matrix_[km.addr] &= ~km.mask;
          if (osd_dialog_on()) osd_process_key(item.vk);
        } else {
          key_matrix_[km.addr] |= km.mask;
        }
      } else {
        ProcessEmulatorKey(&item);
      }
    }
  }
}

void SPC1000::ProcessEmulatorKey(VirtualKeyItem *item) {
  if (item->vk == fabgl::VK_F8) {
    osd_toast("PLAY", 0, 0);
    if (cas_.rfp) FCLOSE(cas_.rfp);
    if (cas_.wfp) FCLOSE(cas_.wfp);
    cas_.button = CAS_PLAY;
    cas_.motor = 1;
  } else if (item->vk == fabgl::VK_F9) {
    osd_toast("RECORD", 0, 0);
    if (cas_.rfp) FCLOSE(cas_.rfp);
    if (cas_.wfp) FCLOSE(cas_.wfp);
    cas_.button = CAS_REC;
    cas_.motor = 1;
  } else if (item->vk == fabgl::VK_F10) {
    osd_toast("STOP", 0, 0);
    if (cas_.rfp) FCLOSE(cas_.rfp);
    if (cas_.wfp) FCLOSE(cas_.wfp);
    cas_.button = CAS_STOP;
    cas_.motor = 0;
  }
}
