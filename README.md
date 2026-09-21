# CHIP-8 Emulator

A CHIP-8 emulator written in **C++** as a learning project focused on
emulation, computer architecture, memory, registers, bitwise operations,
and the **fetch-decode-execute** cycle.

> The goal is not only to make ROMs run, but to understand why they run.

## What I've learned

### CHIP-8 architecture

The CHIP-8 machine I'm emulating contains:

-   4096 bytes (4 KB) of memory
-   16 8-bit registers: `V0` through `VF`
-   16-bit Index Register (`I`)
-   16-bit Program Counter (`PC`)
-   Stack and Stack Pointer (`SP`)
-   8-bit delay timer
-   8-bit sound timer
-   16-key hexadecimal keypad
-   64x32 monochrome display

The `V0`-`VF` registers exist all the time, but each instruction only
uses the registers it needs. `VF` also acts as a flag in several
arithmetic, shift, and drawing operations.

## Memory

CHIP-8 has 4096 addressable bytes:

``` text
0x000
  |
  | reserved/interpreter area
  |
0x050  <- fontset
  |
0x200  <- ROM/program start
  |
  | instructions and data
  |
0xFFF
```

The fontset contains 16 hexadecimal characters (`0`-`F`), with 5 bytes
per character: **80 bytes total**. In this emulator it is loaded
starting at `0x50`. ROMs are loaded starting at `0x200`.

## Program Counter

The Program Counter stores the address of the next instruction to
execute. Since CHIP-8 instructions are 2 bytes long, normal sequential
execution advances like:

``` text
0x200 -> 0x202 -> 0x204 -> 0x206 -> ...
```

Jump, call, return, and skip instructions can change this progression.

## Opcodes

Every CHIP-8 instruction is 16 bits (2 bytes).

If:

``` text
memory[0x200] = 0x3A
memory[0x201] = 0x42
```

the fetched instruction becomes:

``` text
opcode = 0x3A42
```

It can be interpreted as:

``` text
3 A 4 2
| | `--- NN = 0x42
| `----- X = A
`------- instruction family
```

`opcode` therefore contains the complete instruction currently being
processed.

## Fetch -\> Decode -\> Execute

``` text
FETCH -> DECODE -> EXECUTE -> FETCH -> ...
```

**Fetch:** read the two bytes pointed to by `PC` and combine them into
one opcode.

**Decode:** determine which instruction it is and extract fields such as
`X`, `Y`, `N`, `NN`, or `NNN`.

**Execute:** reproduce the instruction's behavior by changing registers,
memory, `PC`, stack, display, etc.

## Bit masks

Common masks used while decoding:

``` cpp
opcode & 0x0FFFu;          // NNN
opcode & 0x00FFu;          // NN / KK
(opcode & 0x0F00u) >> 8u; // X
(opcode & 0x00F0u) >> 4u; // Y
opcode & 0x000Fu;          // N
```

For example:

``` text
0x2ABC & 0x0FFF = 0x0ABC
```

so `NNN = 0xABC`.

This project also made the difference between these operators clear:

``` text
&   -> bitwise AND
&&  -> logical AND
```

### Register index vs register value

If:

``` cpp
uint8_t x = (opcode & 0x0F00u) >> 8u;
```

`x` contains the **index** of the register, not its value.

``` text
x            -> which register
registers[x] -> value stored in that register
```

For example, if `X = A`, then `x = 10`, while `registers[x]` is the
value stored in `VA`.

## Index Register (`I`)

`I` mainly works as a pointer to memory.

``` text
I = 0x350
```

means that the register contains the memory address `0x350`.

A useful mental model is:

``` text
PC -> normally points to instructions
I  -> normally points to data
```

`ANNN` demonstrates this directly:

``` text
A350 -> I = 0x350
```

## Stack and subroutines

The stack stores return addresses for subroutine calls.

Conceptually, `2NNN` (`CALL`) performs:

``` text
stack[SP] = PC
SP++
PC = NNN
```

`00EE` (`RET`) restores a saved address so execution can continue after
the call.

``` text
PC -> execution address
SP -> current stack position
```

## Carry

`VX` and `VY` are 8-bit registers, so their maximum value is `0xFF`.

Example:

``` text
VX = 0xFE
VY = 0x05

0xFE + 0x05 = 0x103
```

The complete result does not fit in 8 bits. A temporary `uint16_t` can
preserve it:

``` cpp
uint16_t sum = registers[x] + registers[y];
```

For `8XY4`:

``` text
sum > 0xFF -> carry -> VF = 1
otherwise          -> VF = 0
```

`VX` receives the low 8 bits of the result.

## Borrow

For `8XY7`:

``` text
VX = VY - VX
```

`VF` indicates whether a borrow occurred:

``` text
VY >= VX -> no borrow -> VF = 1
VY <  VX -> borrow    -> VF = 0
```

## Bit shifts

### Right shift

``` text
10110101 >> 1
01011010
```

The rightmost bit is the **LSB (Least Significant Bit)**:

``` text
10110101
       ^
       LSB
```

It can be isolated with:

``` cpp
value & 0x01u;
```

### Left shift

``` text
10110101 << 1
01101010
```

The leftmost bit is the **MSB (Most Significant Bit)**:

``` text
10110101
^
MSB
```

It can be isolated and normalized to `0` or `1` with:

``` cpp
(value & 0x80u) >> 7u;
```

## CHIP-8 quirks

CHIP-8 has historical implementation differences. One example is `8XY6`
and `8XYE`.

The instruction reference currently followed by this project uses `VY`
as the source and `VX` as the destination:

``` text
8XY6:
VF = LSB of VY
VX = VY >> 1
VY unchanged

8XYE:
VF = MSB of VY
VX = VY << 1
VY unchanged
```

Other implementations shift `VX` directly. Emulator compatibility
therefore sometimes requires choosing a specific behavior and using it
consistently.

## Instructions studied / implemented

  Opcode   Behavior
  -------- ---------------------------------------------------
  `00E0`   Clear display
  `00EE`   Return from subroutine
  `1NNN`   `PC = NNN`
  `2NNN`   Call subroutine at `NNN`
  `3XNN`   Skip if `VX == NN`
  `5XY0`   Skip if `VX == VY`
  `6XKK`   `VX = KK`
  `7XKK`   `VX = VX + KK`
  `8XY0`   `VX = VY`
  `8XY1`   `VX = VX OR VY`
  `8XY2`   `VX = VX AND VY`
  `8XY3`   `VX = VX XOR VY`
  `8XY4`   `VX = VX + VY`, `VF = carry`
  `8XY6`   Shift `VY` right into `VX`; original LSB -\> `VF`
  `8XY7`   `VX = VY - VX`, `VF = NOT borrow`
  `8XYE`   Shift `VY` left into `VX`; original MSB -\> `VF`
  `9XY0`   Skip if `VX != VY`
  `ANNN`   `I = NNN`
  `BNNN`   `PC = NNN + V0`

## ROM loading

ROMs are read as binary data and copied into CHIP-8 memory starting at
`0x200`:

``` text
ROM file
   |
   v
raw bytes
   |
   v
memory[0x200]
memory[0x201]
memory[0x202]
...
```

The Program Counter can then fetch these bytes as instructions.

## C++ concepts practiced

-   `uint8_t` and `uint16_t`
-   arrays
-   classes and objects
-   constructors
-   initializer lists
-   binary file I/O
-   hexadecimal and binary representation
-   bitwise operators
-   bit masks
-   left/right shifts
-   integer overflow
-   random-number generation
-   `std::chrono`
-   `std::default_random_engine`
-   `std::uniform_int_distribution`

## The main idea

At first:

``` text
3A42
```

looks like an arbitrary hexadecimal number.

After understanding CHIP-8 decoding:

``` text
3 A 4 2
| | `--- NN = 0x42
| `----- X = A -> VA
`------- 3XNN
```

it becomes:

``` text
if VA == 0x42:
    skip the next instruction
```

That summarizes the central idea behind the emulator:

``` text
BYTES IN MEMORY
       |
       v
     FETCH
       |
       v
    OPCODE
       |
       v
    DECODE
       |
       v
    EXECUTE
       |
       v
CHANGE THE EMULATED MACHINE'S STATE
```

The emulator does not execute CHIP-8 instructions natively. It reads
them and **reproduces their defined behavior in C++**.

## Current progress

The project is currently focused on implementing the instruction set.

Next steps:

-   Finish the remaining opcodes
-   `CXNN` random instruction
-   `DXYN` sprite drawing and collision
-   `EX**` keypad instructions
-   `FX**` instructions
-   Delay and sound timers
-   Keyboard input
-   Display integration
-   Main emulation loop
-   ROM testing
-   Compatibility testing and quirks

## Long-term goal

CHIP-8 is my first step into emulator development and low-level
programming.

The goal is not just:

> Make a ROM run.

It is to understand:

> **Why does it run?**
