# Audio-Effect-Processor

One-line description: A real-time audio processing application integrating SDL and OpenGL for audio effects and visualization.

Summary: This C++ application uses the SDL library for audio handling and OpenGL (via freeglut) for visualization to create a real-time audio effects processor. It defines base and derived classes for sound effects such as Pitch Shift and Reverb, which are applied to audio buffers. The audio callback function handles audio data processing using mutexes for thread-safe operations and integrates performance measurements such as processing time and FPS calculation. The application also includes OpenGL functions to visualize audio waveforms, providing a graphical display of the input and processed audio signals. The main function sets up the audio and graphics environments and runs the processing loop, displaying debug information regarding performance metrics like FPS and audio processing times.
