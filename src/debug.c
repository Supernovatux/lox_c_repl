#include "debug.h"
#include "isocline.h"

void disassembleChunk(Chunk *chunk, const char *name) {
  ic_printf("== %s ==\n", name);

  for (int offset = 0; offset < chunk->count;) {
    offset = disassembleInstruction(chunk, offset);
  }
}
static int simpleInstruction(const char *name, int offset) {
  ic_printf("%s\n", name);
  return offset + 1;
}
static int constantInstruction(const char *name, Chunk *chunk, int offset) {
  uint8_t constant = chunk->code[offset + 1];
  ic_printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  ic_printf("'\n");
  return offset + 2;
}
int disassembleInstruction(Chunk *chunk, int offset) {
  ic_printf("%04d ", offset);
  if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) {
    ic_printf("   | ");
  } else {
    ic_printf("%4d ", chunk->lines[offset]);
  }
  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
  case OP_CONSTANT:
    return constantInstruction("OP_CONSTANT", chunk, offset);
  case OP_RETURN:
    return simpleInstruction("OP_RETURN", offset);
  default:
    ic_printf("Unknown opcode %d\n", instruction);
    return offset + 1;
  }
}