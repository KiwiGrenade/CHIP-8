# Notes
### Interesting youtube [comments](https://www.youtube.com/watch?v=_h5lBdQVJLg) by user ```8942th``` on opcodes: ```DXYN```, ```FX0A``` etc.
#1
```
[...] display wait is not quite as complicated as it sounds. The most rudimentary form of it that you can implement, is to ensure that DXYN will only be allowed to run when it's the first instruction of a frame, meaning that in any other instance, the instruction loop will just spin in place, not doing any work. Timers still decrement as normal of course, you're never meant to pause those.

Implementing a more accurate version to account for the "big draws" that'd make the display wait 2 frames is, unfortunately, a lot more involved. An approximation is possible, but not accurate, and not very useful either. You'd need a full-on VIP core that emulates everything in depth to be able to calculate instruction timings and figuring out if you need to wait for a double vblank before the draw is allowed to proceed. Scary stuff. If you want to work the simple short display wait, strongly recommend to just ensure that dxyn is only allowed to be drawn as the first instruction of a frame. If you have some interrupt system in place, you can of course mesh it in there somewhere, depends on your exact implementation.

On the matter of the FX0A, you're expected to halt the instruction loop essentially and await for input. The first key that is released is then fed to Vx, and the instruction loop resumes. 
The easiest way to check for key release events, if the libraries you use (if any) don't offer an easy option, is to implement two arrays of keys. Each frame, the first array will inherit the values of the second array, and the second array will have all 16 of its values updated with the current status of the hex keypad (1 if pressed, 0 if not). You can then use these two arrays to compare and figure out if a particular key had a press or release event, by going from 0 to 1 and from 1 to 0 from the previous frame to the current respectively.
Throw that check into the FX0A's key scan, break the loop for the first one that tests positive for a release event, and you'll have one happy test rom on your hands.

You can, of course, opt for the lazy (though snappier) approach of proceeding with a key press instead, but several roms that follow up with EX9E and EXA1 instructions after the FX0A would run into false inputs because your finger is still holding the key down, there's just no way you'll tap a key for a single frame as a human. As you can tell, an FX0A that works with a key release doesn't suffer the same fate.
 ```

#2
```
[...] I've also found the default FX0A behavior sluggish for that exact same reason. It took me a while to polish it but at this point I've arrived at a setup that works flawlessly, and it passes all the same tests that the on-release version would pass. It might be a bit much to explain here in a comment but I'll try.

The first thing to note is that in my setup, the instruction loop occurs first -- it will loop X instructions each time (SPEED variable), and quit early if the SPEED variable that dictates the amount of instructions to run isn't > 0. This part's important. When an FX0A is detected, I cache the Vx register reference to a variable, and call an interrupt function that sets the type of interrupt (in my case 0 is single-frame delay, also used in display wait, 1 is for jump loop detection which stops the emulator for good, and 2 which is dedicated for FX0A) and inverts the SPEED to a negative.

When this occurs, the instruction loop quits early since the SPEED is no longer positive. After the instruction loop itself however, follow three other functions in sequence, they're part of the main loop, and said main loop only runs when SPEED != 0. The first  function updates the input values for the next frame (we'll go over that), the second decrements timers (nothing special here), and the last checks the interrupt type and takes certain actions (we'll also go over this).

The magic here now occurs on multiple layers. I have THREE key input variables (I clump each key into a composite 16-bit variable, so I can check all of them at once without looping and checking them individually). One is KP (keysPrevious), another is KC (keysCurrent) and the last is IK (inputKeys). The first holds the keys from the previous frame, it is always assigned the values from KC when the input update function is called, then KC itself is updated to flip all the bits to 0 or 1, depending on what keys are being held down during the frame. Lastly IK itself is updated at the end by doing some bitwise math between KP and KC to detect and return the bits of any keys that underwent a release event during the two frames. If they were, those same bits are subtracted (bitwise at all times) from IK itself, clearing them, any others are not touched.

In the last stage where the interrupt state evaluation function runs, I do slightly similar bitwise math to save a temporarily variable that holds all the bits of keys that underwent a press event during the two frames instead. Then, if I subtract the IK bits from that temp var and some bit(s) remain, I do the following which was previously part of the FX0A instruction's work:
Bitwise OR the bits from the temp var to IK (so we know they are used), then set VX (from the cached reference) to the position of the lowest enabled bit of N (I use log2 here for the math). Lastly, I clear the interrupt state variable, and set the X variable for instruction speed back to a positive value, allowing the instruction loop to resume again in the next frame.

I hope the general process so far is understandable to a degree, even though I haven't pasted actual code :P

The last part is to secure the EX9E and EXA1 instructions from misfires due to the new FX0A behavior, and that's fairly simple. 

By default (in my setup anyway), all I had to do previously is bitwise AND the KC variable with 1 shifted left to the value of VX to check if a particular key was being pressed. Now, you just adjust the KC part of the equation by first subtracting the bits of IK from it, effectively ensuring that even if a key is pressed, it would not be detected while IK holds its value. Since IK is updated every frame to clear any bits of keys that underwent a release event, this makes a flawless input loop.

Let me know if you want to see some actual code on all this :D

As for key repeat, I'd say not to bother if you plan to implement it from the interpreter's side of things. It would take a long time to figure out how to tie in timers with your input variables and ensure there's no weird behaviors. It's eldritch enough working around the system to get flawless on-press behavior, and rom makers could implement input repeat on their own as part of the rom program itself if they wanted it. Typically they utilize the EX9E and EXA1 if an input needs to be consistent or repeated, or when non-locking input is required (like in cases where animations need to continue running while input is awaited).
```
## Translating C++ + Qt with Emscripten and CMake
1. [ ] Install Qt
2. [ ] Install Emscripten (add the source command to ~/.bashrc)
3. [ ] Install Ninja
4. [ ] Follow the instructions [here](https://doc.qt.io/qt-6/wasm.html).
You have to unpack qt-everywhere int a folder with user acces. !DO NOT USE SUDO!. If you've picked a "Qt for Desktops" package you need to install module "RemoteObject" manually. You can do that with <Qt_root>/Maintanance 
