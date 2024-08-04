# CHIP-8
CHIP-8 emulator in C++.
POC for my Bachelor's Degree Thesis.

## Opcodes
- [ ] ```0x00E0: Clear screen```
- [x] ```0x00EE: Return from subroutine```
- [x] ```0x1NNN: Jump to location```
- [x] ```0x2NNN: Call a subroutine at address NNN```
- [x] ```0x3XKK: Skip next instr. if V[X] == KK```
- [x] ```0x4XKK: Skip next instr. if V[X] != KK```
- [x] ```0x5XY0: Skip next instr. if V[X] == V[Y]```
- [ ] ```0x6XKK: V[X] = KK```
- [x] ```0x7XKK: V[X] += KK```
- [x] ```0x8XY0: V[X] = V[Y]```
- [x] ```0x8XY1: V[X] OR V[Y]```
- [x] ```0x8XY2: V[X] AND V[Y]```
- [x] ```0x8XY3: V[X] XOR V[Y]```
- [x] ```0x8XY4: V[X] ADD V[Y]```
- [x] ```0x8XY5: V[X] SUB V[Y]```
- [x] ```0x8XY6: V[X] = V[X] / 2```
- [x] ```0x8XY7: V[X] SUBN V[X]```
- [x] ```0x8XYE: V[X] = V[X] * 2```
- [x] ```0x9XY0: Skip next instr. if V[X] != V[Y]```
- [ ] ```0xANNN: I = NNN```
- [ ] ```0xBNNN: pc = NNN + V[0x0]```
- [ ] ```0xCXKK: V[X] = random byte AND KK```
- [ ] ```0xDXYN: Display n-byte sprite starting at memory location I (V[X, V[Y]), V[F] = collision```
- [ ] ```0xEX9E: Skip next instr. if key with value of V[X] is pressed```
- [ ] ```0xEXA1: Skip next instr. if key with value of V[X] is NOT pressed```
- [ ] ```0xFX07: V[X] = delay_timer```
- [x] ```0xFX0A: Wait for key press, store value of pressed key in V[X]```
- [ ] ```0xFX15: delay_timer = V[X]```
- [ ] ```0xFX18: sound_timer = V[X]```
- [x] ```0xFX1E: I = I + V[X]```
- [ ] ```0xFX29: I + location_of_sprite_for_digit_V[X]```
- [x] ```0xFX33: Store BCD representation of V[X] in memory locations I, I+1 and I+2```
- [x] ```0xFX55: Store registers V[0x0] through V[X] in memory starting at location I```
- [x] ```0xFX65: Read registers V[0x0] through V[X] from memory starting at location I```
## Roms
### Tests
- [x] CHIP-8 splash screen
- [x] IBM logo
- [x] Corax+ opcode test
- [x] Flags test
- [ ] Beep test
- [ ] Keypad test
- [ ] Quirks test

### Games
T.B.A.

## Side Quests
- [ ] Add debug mode
- [ ] Add telemetry
- [ ] Translate C++ code into WebAssembly
- [ ] Host assembly code on server

## Materials
- [Timendus/chip8-test-suite](https://github.com/Timendus/chip8-test-suite)
- [CHIP-8 Specification](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#1.0)
- [CHIP-8 Wikipedia Page](https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description)
- [CHIP-8 Emulator Guide in C++](https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter)
- [CHIP-8 Emulator Guide](https://chip-8.github.io/links/#emulatorinterpreter-development)
