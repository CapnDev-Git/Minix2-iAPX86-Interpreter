#include "print_patterns.h"
void printsw(size_t a, char *instr, char *ea, unsigned char LSB4, size_t *ds) {
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char rm = text_mem[a + 1] & 0b111;
  get_adm(a + 2, mod, rm, ea, ds);

  char line[256]; // Buffer to store the formatted string

  switch (LSB4 & 0b11) {
  case 0b00:
    print4b(text_mem, a, 2 + *ds + 1, line);
    sprintf(line + strlen(line), "%s %s%s, %x", instr, mod != 3 ? "byte " : "",
            mod == 3 ? REG8[rm] : ea, text_mem[a + 2 + *ds]);
    break;

  case 0b01:
    print4b(text_mem, a, 2 + *ds + 2, line);
    sprintf(line + strlen(line), "%s %s, %04x", instr,
            mod == 3 ? REG16[rm] : ea, *(uint16_t *)&text_mem[a + 2 + *ds]);
    break;

  case 0b10:
    print4b(text_mem, a, 2 + *ds, line);
    sprintf(line + strlen(line), "%s %s, %02x", instr, mod == 3 ? REG8[rm] : ea,
            *(uint8_t *)&text_mem[a + 2]);
    break;

  case 0b11:
    print4b(text_mem, a, 2 + *ds + 1, line);
    int8_t imm8 = text_mem[a + 2 + *ds];
    sprintf(line + strlen(line), "%s %s, %s%x", instr,
            mod == 3 ? REG16[rm] : ea, imm8 < 0 ? "-" : "",
            imm8 < 0 ? -imm8 : imm8);
    break;
  }

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void printvw(size_t a, char *instr, unsigned char LSB4, size_t *ds) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));
  unsigned char v = (LSB4 >> 1) & 0b1;
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char rm = text_mem[a + 1] & 0b111;

  char line[256]; // Buffer to store the formatted string
  get_adm(a + 2, mod, rm, ea, ds);
  print4b(text_mem, a, 2 + *ds, line);

  if (!v) {
    if (!w) {
      if (mod == 0b11)
        sprintf(line + strlen(line), "%s %s, 1", instr, REG8[rm]);
      else
        sprintf(line + strlen(line), "%s %s, 1", instr, ea);
    } else {
      if (mod == 0b11)
        sprintf(line + strlen(line), "%s %s, 1", instr, REG16[rm]);
      else
        sprintf(line + strlen(line), "%s %s, 1", instr, ea);
    }
  } else {
    if (!w) {
      if (mod == 0b11)
        sprintf(line + strlen(line), "%s %s, cl", instr, REG8[rm]);
      else
        sprintf(line + strlen(line), "%s byte %s, cl", instr, ea);
    } else {
      if (mod == 0b11)
        sprintf(line + strlen(line), "%s %s, cl", instr, REG16[rm]);
      else
        sprintf(line + strlen(line), "%s %s, cl", instr, ea);
    }
  }

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
  free(ea);
}

void printdw(size_t a, char *instr, unsigned char LSB4, size_t *ds) {
  char *ea = malloc(EA_STRING_SIZE * sizeof(unsigned char));

  unsigned char d = (LSB4 >> 1) & 0b1;
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char reg = (text_mem[a + 1] >> 3) & 0b111;
  unsigned char rm = text_mem[a + 1] & 0b111;

  char line[256]; // Buffer to store the formatted string
  get_adm(a + 2, mod, rm, ea, ds);
  print4b(text_mem, a, 2 + *ds, line);

  sprintf(line + strlen(line), "%s %s, %s", instr,
          d ? (w ? REG16[reg] : REG8[reg])
            : (mod == 3 ? (w ? REG16[rm] : REG8[rm]) : ea),
          d ? (mod == 3 ? (w ? REG16[rm] : REG8[rm]) : ea)
            : (w ? REG16[reg] : REG8[reg]));

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
  free(ea);
}

void print_immtoacc(size_t a, char *instr, unsigned char w) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 2 + w, line);

  if (w) {
    sprintf(line + strlen(line), "%s ax, %04x", instr,
            text_mem[a + 1] + (text_mem[a + 2] << 8));
  } else {
    sprintf(line + strlen(line), "%s al, %x", instr, text_mem[a + 1]);
  }

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_immtorm(size_t a, char *instr, char *ea, unsigned char LSB4,
                   size_t *ds) {
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char rm = text_mem[a + 1] & 0b111;
  char line[256]; // Buffer to store the formatted string
  get_adm(a + 2, mod, rm, ea, ds);
  print4b(text_mem, a, 3 + w + *ds, line);

  if (w)
    sprintf(line + strlen(line), "%s %s, %04x", instr,
            mod == 3 ? REG16[rm] : ea, *(uint16_t *)&text_mem[a + 2]);
  else
    sprintf(line + strlen(line), "%s %s, %02x", instr, mod == 3 ? REG8[rm] : ea,
            *(uint8_t *)&text_mem[a + 2]);

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_rmtoimm(size_t a, char *instr, char *ea, unsigned char LSB4,
                   size_t *ds) {
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char rm = text_mem[a + 1] & 0b111;
  char line[256]; // Buffer to store the formatted string
  get_adm(a + 2, mod, rm, ea, ds);
  print4b(text_mem, a, 3 + w + *ds, line);

  if (w) {
    if (mod == 0b11) {
      sprintf(line + strlen(line), "%s %s, %04x", instr, REG16[rm],
              *(uint16_t *)&text_mem[a + 2]);
    } else {
      sprintf(line + strlen(line), "%s %s, %04x", instr, ea,
              *(uint16_t *)&text_mem[a + 2 + *ds]);
    }
  } else {
    if (mod == 0b11) {
      sprintf(line + strlen(line), "%s %s, %02x", instr, REG8[rm],
              *(uint8_t *)&text_mem[a + 2]);
    } else {
      sprintf(line + strlen(line), "%s %s, %02x", instr, ea,
              *(uint8_t *)&text_mem[a + 2]);
    }
  }

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_rm16(size_t a, char *instr, unsigned char rm) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 1, line);

  sprintf(line + strlen(line), "%s %s", instr, REG16[rm]);

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_condjmps(size_t a, char *instr) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 2, line);
  sprintf(line + strlen(line), "%s %04x", instr,
          IP + (signed char)text_mem[a + 1]);

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_repmanip(size_t a, char *instr, unsigned char w) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 2, line);
  sprintf(line + strlen(line), "rep %s%c", instr, w ? 'w' : 'b');

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_single(size_t a, char *instr, size_t il) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, il, line);
  sprintf(line + strlen(line), "%s", instr);

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_portfixed(size_t a, char *instr, unsigned char w) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 2, line);
  sprintf(line + strlen(line), "%s %s, %x", instr, w ? "ax" : "al",
          text_mem[a + 1]);

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_portvar(size_t a, char *instr, unsigned char w) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 1, line);
  sprintf(line + strlen(line), "%s %s, dx", instr, w ? "ax" : "al");

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_loop(size_t a, char *instr) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 2, line);
  sprintf(line + strlen(line), "%s %04x", instr,
          IP + *(int8_t *)&text_mem[a + 1]);

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_calljmp(size_t a, char *instr) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 3, line);
  sprintf(line + strlen(line), "%s %04x", instr,
          IP + *(int16_t *)&text_mem[a + 1]);

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_stringmanip(size_t a, char *instr, unsigned char w) {
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 1, line);
  sprintf(line + strlen(line), "%s%c", instr, w ? 'w' : 'b');

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_fw(size_t a, char *instr, char *ea, unsigned char LSB4, size_t *ds) {
  unsigned char w = LSB4 & 0b1;
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char rm = text_mem[a + 1] & 0b111;
  char line[256]; // Buffer to store the formatted string
  print4b(text_mem, a, 2 + *ds, line);

  if (!w) {
    if (mod == 0b11)
      sprintf(line + strlen(line), "%s %s", instr, REG8[rm]);
    else
      sprintf(line + strlen(line), "%s %s", instr, ea);
  } else {
    if (mod == 0b11)
      sprintf(line + strlen(line), "%s %s", instr, REG16[rm]);
    else
      sprintf(line + strlen(line), "%s %s", instr, ea);
  }

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}

void print_frm(size_t a, char *instr, char *ea, size_t *ds) {
  unsigned char mod = (text_mem[a + 1] >> 6) & 0b11;
  unsigned char rm = text_mem[a + 1] & 0b111;
  char line[256]; // Buffer to store the formatted string
  get_adm(a + 2, mod, rm, ea, ds);
  print4b(text_mem, a, 2 + *ds, line);

  sprintf(line + strlen(line), "%s %s", instr, mod == 3 ? REG16[rm] : ea);

  // Add the line to the lsit of ASM ligns
  strncpy(ASM[a], line, sizeof(ASM[a]));
  ASM_MAX_INDEX = a;
}