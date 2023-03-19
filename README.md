## clock multiplier
This is a clock multiplier made for musical purpose, such as synthesizer/modular-systems or other things that would send out or receive 5v pulses. The current code is written for a ATTiny85, but should work fine on any Arduino etc.

WARNING!: this is made only using micros() (no interrupts)<br />
max input frequency is around 7Hz when no multipliers are added (max output of 14Hz/71ms)<br />
max input frequency is around 0.350Hz when all multipliers are added (max output of 70Hz/10ms)<br />

<img src="https://www.simonjuhl.net/wp-content/uploads/2020/03/IMG_0597-768x432.jpg" alt="front" width="480px">
<img src="https://www.simonjuhl.net/wp-content/uploads/2020/03/IMG_0602-1024x576.jpg" alt="back" width="480px">

The program measures the time between each pulse input, and outputs a repeating pulse with an interval defined by the variable 'mult'. There are 3 more inputs/outputs avalible if using and ATTiny85 these are currently used for changing mult, but could be replaced by an potentiometer for a wider range of tempos.

*Visit my [website](https://www.simonjuhl.net/) to see more projects or contact me if you have any questions.*
