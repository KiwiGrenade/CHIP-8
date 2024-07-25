# CHIP-8
CHIP-8 emulator in C++.
POC for my Bachelor's Degree Thesis.

## Opcodes
- [ ] ```0x00E0: Clear screen```
- [ ] ```0x00EE: Return from subroutine```
- [ ] ```0x1NNN: Jump to location```
- [ ] ```0x2NNN: Call a subroutine at address NNN```
- [ ] ```0x3XKK: Skip next instr. if V[X] == KK```
- [ ] ```0x4XKK: Skip next instr. if V[X] != KK```
- [ ] ```0x5XY0: Skip next instr. if V[X] == V[Y]```
- [ ] ```0x6XKK: V[X] = KK```
- [ ] ```0x7XKK: V[X] += KK```
- [ ] ```0x8XY0: V[X] = V[Y]```
- [ ] ```0x8XY1: V[X] OR V[Y]```
- [ ] ```0x8XY2: V[X] AND V[Y]```
- [ ] ```0x8XY3: V[X] XOR V[Y]```
- [ ] ```0x8XY4: V[X] ADD V[Y]```
- [ ] ```0x8XY5: V[X] SUB V[Y]```
- [ ] ```0x8XY6: V[X] = V[X] / 2```
- [ ] ```0x8XY7: V[X] SUBN V[X]```
- [ ] ```0x8XYE: V[X] = V[X] * 2```
- [ ] ```0x9XY0: Skip next instr. if V[X] != V[Y]```
- [ ] ```0xANNN: I = NNN```
- [ ] ```0xBNNN: pc = NNN + V[0x0]```
- [ ] ```0xCXKK: V[X] = random byte AND KK```
- [ ] ```0xDXYN: Display n-byte sprite starting at memory location I (V[X, V[Y]), V[F] = collision```
- [ ] ```0xEX9E: Skip next instr. if key with value of V[X] is pressed```
- [ ] ```0xEXA1: Skip next instr. if key with value of V[X] is NOT pressed```
- [ ] ```0xFX07: V[X] = delay_timer```
- [ ] ```0xFX0A: Wait for key press, store value of pressed key in V[X]```
- [ ] ```0xFX15: delay_timer = V[X]```
- [ ] ```0xFX18: sound_timer = V[X]```
- [ ] ```0xFX1E: I = I + V[X]```
- [ ] ```0xFX29: I + location_of_sprite_for_digit_V[X]```
- [ ] ```0xFX33: Store BCD representation of V[X] in memory locations I, I+1 and I+2```
- [ ] ```0xFX55: Store registers V[0x0] through V[X] in memory starting at location I```

## Roms
### Tests
- [ ] CHIP-8 splash screen
- [ ] IBM logo
- [ ] Corax+ opcode test
- [ ] Flags test
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
- [CHIP-8 Emulator Guide in C++](https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)
- [CHIP-8 Emulator Guide](https://chip-8.github.io/links/#emulatorinterpreter-development)
