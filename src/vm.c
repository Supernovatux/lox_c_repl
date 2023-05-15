#include "vm.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "isocline.h"
VM vm;
static void resetStack() { vm.stackTop = vm.stack; }
void initVM() { resetStack(); }

void freeVM() {}
void push(Value value) {
  *vm.stackTop = value;
  vm.stackTop++;
}
Value pop() {
  vm.stackTop--;
  return *vm.stackTop;
}
static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op)                                                          \
  do {                                                                         \
    double b = pop();                                                          \
    vm.stackTop[-1] = vm.stackTop[-1] op b;                                    \
  } while (false)

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    ic_printf("          ");
    for (Value *slot = vm.stack; slot < vm.stackTop; slot++) {
      ic_printf("[ ");
      printValue(*slot);
      ic_printf(" ]");
    }
    ic_printf("\n");
    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
    case OP_ADD:
      BINARY_OP(+);
      break;
    case OP_SUBTRACT:
      BINARY_OP(-);
      break;
    case OP_MULTIPLY:
      BINARY_OP(*);
      break;
    case OP_DIVIDE:
      BINARY_OP(/);
      break;
    case OP_NEGATE:
      vm.stackTop[-1] = -vm.stackTop[-1];
      break;
    case OP_RETURN: {
      printValue(pop());
      ic_printf("\n");
      return INTERPRET_OK;
    }
    case OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      push(constant);
      break;
    }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}
InterpretResult interpret(const char *source) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(source, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  vm.chunk = &chunk;
  vm.ip = vm.chunk->code;

  InterpretResult result = run();

  freeChunk(&chunk);
  return result;
}
