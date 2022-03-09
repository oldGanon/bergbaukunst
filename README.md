# BergBauKunst 

![screenshot][screenshot]

---

# Description

A software rendered minecraft clone.
Utilizing AVX SIMD instruction and multithreading to implement a modern tile-based rasterizer on the CPU.
Supports perspective correct texturing and depth buffering.

---

# Why?

Started out as a way to teach my brother programming in C combined with some game and graphics programming concepts.

I personally used it as a practice project for parallel programming using threads and SIMD for the software rendering,
network programming using sockets for client server communication, and understanding and designing noise functions for procedural generation.

Another goal was using minimal dependencies, as such the C standard library is not linked. Microsoft Windows APIs are used to interact with the operating system for window creation, threads, audio, memory, timing, sockets, and user input.

[screenshot]: data/screenshots/0.png "Screenshot"
