#include <SDL.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <chrono>
#include <cmath>

// Assuming a simple audio format for demonstration
constexpr int AUDIO_SAMPLING_RATE = 44100;
constexpr int AUDIO_CHANNELS = 2; // Stereo
constexpr SDL_AudioFormat AUDIO_FORMAT = AUDIO_S16SYS; // 16-bit system-endian
constexpr int BUFFER_SIZE = 4096; // Size of audio buffer

// Sound Effect Base Class
class SoundEffect {
public:
    virtual ~SoundEffect() {}
    virtual void applyEffect(std::vector<Sint16>& buffer) = 0; // Using Sint16 for 16-bit samples
};

// Pitch Shift Effect (Simplified Implementation)
class PitchShift : public SoundEffect {
public:
    void applyEffect(std::vector<Sint16>& buffer) override {
        // Simplified: Modify pitch by duplicating each sample (very basic and not accurate)
        for(size_t i = 0; i < buffer.size(); ++i) {
            buffer[i] = buffer[i / 2];
        }
    }
};

// Reverb Effect (Simplified Implementation)
class Reverb : public SoundEffect {
public:
    void applyEffect(std::vector<Sint16>& buffer) override {
        // Simplified: Add a simple echo effect as a form of reverb
        const size_t delaySamples = AUDIO_SAMPLING_RATE / 10; // 100ms delay
        for(size_t i = delaySamples; i < buffer.size(); ++i) {
            buffer[i] += buffer[i - delaySamples] / 2;
        }
    }
};

std::vector<Sint16> audioBufferIn; // Input audio buffer
std::vector<Sint16> audioBufferOut; // Output audio buffer
std::mutex audioMutex; // Mutex for audio buffer access
std::chrono::steady_clock::time_point lastFrameTime = std::chrono::steady_clock::now();
int frameCount = 0;
double fps = 0.0;
double lastProcessTime = 0.0; // Time is taken for the last audio processing

// SDL Audio Callback
void audioCallback(void* userdata, Uint8* stream, int len) {
    std::lock_guard<std::mutex> lock(audioMutex);
    
    // Convert Uint8* to Sint16* to match our audio format
    Sint16* samples = reinterpret_cast<Sint16*>(stream);
    size_t sampleCount = len / 2; // Because we're using 16-bit samples

    // Simulate loading audio data into 'audioBufferIn'
    // In a real application, you would likely fill this buffer from an actual audio source
    audioBufferIn.resize(sampleCount);
    std::copy(samples, samples + sampleCount, audioBufferIn.begin());

    // Processing (apply effects)
    auto start = std::chrono::steady_clock::now();
    
    // Example: Apply Pitch Shift and then Reverb
    PitchShift pitchShiftEffect;
    Reverb reverbEffect;
    pitchShiftEffect.applyEffect(audioBufferIn);
    reverbEffect.applyEffect(audioBufferIn);
    
    // Copy processed buffer back to output
    std::copy(audioBufferIn.begin(), audioBufferIn.end(), samples);
    
    auto end = std::chrono::steady_clock::now();
    lastProcessTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

// OpenGL Display Function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Implement drawing logic here
    // Example: Draw input waveform in blue and output waveform in green

    glutSwapBuffers();
    calculateFPS();
    displayDebugInfo();
}

// Function to calculate FPS and display debug information
void calculateFPS() {
    frameCount++;
    auto now = std::chrono::steady_clock::now();
    double seconds = std::chrono::duration_cast<std::chrono::duration<double>>(now - lastFrameTime).count();
    if(seconds >= 1.0) {
        fps = frameCount / seconds;
        frameCount = 0;
        lastFrameTime = now;
        // Display FPS in the console for simplicity
        std::cout << "FPS: " << fps << " | Last Audio Process Time: " << lastProcessTime << "ms" << std::endl;
    }
}

void displayDebugInfo() {
    // Additional debug info could be displayed on screen
    // For simplicity, we'll log to console
}

// Initialize SDL_audio
void initAudio() {
    SDL_AudioSpec wantedSpec;
    SDL_zero(wantedSpec);
    wantedSpec.freq = AUDIO_SAMPLING_RATE;
    wantedSpec.format = AUDIO_FORMAT;
    wantedSpec.channels = AUDIO_CHANNELS;
    wantedSpec.samples = BUFFER_SIZE;
    wantedSpec.callback = audioCallback;

    if (SDL_OpenAudio(&wantedSpec, NULL) < 0) {
        std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_PauseAudio(0); // Start audio
}

// Initialize freeglut
void initGraphics(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Sound Wave Display");
    glutDisplayFunc(display);
    // Additional glut setup as needed
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    initGraphics(argc, argv);
    initAudio();
    
    glutMainLoop();
    SDL_Quit();
    return 0;
}