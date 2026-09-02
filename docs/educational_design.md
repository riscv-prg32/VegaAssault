# Educational Design

## Learning objectives
Students should be able to explain the C execution model, static storage, structures and arrays, finite-state machines, integer arithmetic, collision detection, pseudo-random sequences, sprite representation, audio sequencing, ABI boundaries, cross compilation, and memory budgets.

## Line-by-line commentary
The C sources use `// EDUCATIONAL:` annotations adjacent to every nonblank source line. The intention is not to replace lectures or language references; it is to make each implementation decision inspectable while a student single-steps or reads a diff. Generated sprite tables are identified as generated data so students learn to distinguish source algorithms from derived artifacts.

## Suggested laboratory progression
A first lab traces initialization and input. A second changes one enemy rule and predicts its effect. A third studies RGB565 and sprite memory. A fourth modifies a musical sequence and stereo pan. A fifth measures code/data growth and enforces the 128 KiB cartridge limit. A final lab builds the same cartridge for QEMU validation and physical ESP32-C6 deployment, then documents observed differences.
