#include "spc1000.h"

#define I_PERIOD 4000
#define INTR_PERIOD 16.666

extern uint8_t rom[];

extern SPC1000 spc;

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
}

namespace {

int readByte(void *context, int address) {
  return ((SPC1000*)context)->ReadMem(address);
}

void writeByte(void *context, int address, int value) {
  ((SPC1000*)context)->WriteMem(address, value);
}

int readWord(void *context, int addr) {
  return readByte(context, addr) | (readByte(context, addr + 1) << 8);
}

void writeWord(void *context, int addr, int value) {
  writeByte(context, addr, value & 0xFF);
  writeByte(context, addr + 1, value >> 8);
}

int readIO(void *context, int addr) {
  SPC1000 *m = (SPC1000 *)context;
  return m->ReadIO(addr);
}

void writeIO(void * context, int addr, int value) {
  SPC1000 *m = (SPC1000 *)context;
  m->WriteIO(addr, value);
}

int32_t GetTicks() {
  return esp_timer_get_time()/1000;
}

} // namespace

SPC1000::SPC1000() {}
SPC1000::~SPC1000() {}

void SPC1000::Init() {
  memset(key_matrix_, 0xff, sizeof key_matrix_);
  memset(mem_, 0, 65536);

  LoadMem(0, rom, 0x8000);
  mc6847_.Init(io_);
  ay38910_.Init(&sound_generator_);

  keyboard_.begin(PS2Preset::KeyboardPort0);

  ResetZ80(&cpu_);
  cpu_.ICount = I_PERIOD;
  tick = 0;
  refrTimer = 0;
  refrSet = 0;

  ay38910_.Loop(0);

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
  key_table_[fabgl::VK_D] = { 3, 0x40 };
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
  key_table_[fabgl::VK_Y] = { 5, 0x40 };
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

void SPC1000::LoadMem(int address, uint8_t const * data, int length) {
  memcpy(mem_+address, data, length);

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

void SPC1000::WriteIO(int addr, int data) {
  if (0x0000 <= addr && addr < 0x2000) {
    io_[addr] = data;
    return;
  } else if (addr == 0x4000) {
    ay38910_.WrCtrl(data);
  } else if (addr == 0x4001) {
    ay38910_.Write(data);
  }
}

int SPC1000::ReadIO(int addr) {
  if (0x0000 <= addr && addr < 0x2000) {
    return io_[addr];
  } else if (0x8000 <= addr && addr <= 0x8009) {
    return KeyIOMatrix(addr-0x8000);
  } else if ((addr & 0xfffe) == 0x4000) {
    return ay38910_.RdData();
  }
  return 0;
}


#define KBD_PERIOD 1000*10

void SPC1000::Run() {
  int turboState = 0;
  int prevTurboState = 0;

  while (true) {
    if (cpu_.ICount <= 0) {
      tick++;
      cpu_.ICount += I_PERIOD;
      intrTime -= 1.0;
      if (intrTime < 0) {
        intrTime += INTR_PERIOD;
        if (cpu_.IFF & IFF_EI) {
          cpu_.IFF |= IFF_IM1 | IFF_1;
          IntZ80(&cpu_, 0);
        }
      }

      simul.curTick = GetTicks() - simul.baseTick;
      if (simul.curTick - simul.prevTick > 0) {
        // TODO: call AY-3-8910 routine periodically
        //
        simul.prevTick = simul.curTick;
      }

      turboState = cas.motor || turbo;
      if (prevTurboState && !turboState && simul.curTick < tick) {
        tick = simul.curTick; // adjust timing if turbo state turned off
      }
      prevTurboState = turboState;

      while (!turboState && (simul.curTick < tick)) {
        vTaskDelay(1);
        simul.curTick = GetTicks() - simul.baseTick;
        // Loop8910(&spc.IO.ay8910, curTick - prevTick);
      }
    }
    ExecZ80(&cpu_);
  }

/*
    kbd_timer -= delta;
    if (kbd_timer < 0) {
      kbd_timer = KBD_PERIOD;
      auto kbd = fabgl::PS2Controller::keyboard();
      if (kbd->virtualKeyAvailable()) {
        VirtualKeyItem item;
        if (kbd->getNextVirtualKey(&item)) {
          KeyMat km = KeyMatFromVirt(item.vk);
          if (km.addr >= 0) {
            if (item.down)
              key_matrix_[km.addr] &= ~km.mask;
            else
              key_matrix_[km.addr] |= km.mask;
          }
        }
      }
    }

    psg_timer -= delta;
    if (psg_timer < 0) {
      ay38910_.Loop((ts - prev_psg_ts)/1000);
      prev_psg_ts = ts;
      psg_timer += PSG_PERIOD;
    }
    prev_ts = ts;
  */
}

KeyMat SPC1000::KeyMatFromVirt(fabgl::VirtualKey vk) {
  return key_table_[static_cast<int>(vk)];
}
