#include <stdint.h>

#define WINDOW_SIZE 512        // Size of each analysis window
#define ENERGY_THRESHOLD 0.1   // Energy threshold for beat detection
#define SAMPLE_RATE 44100      // Sample rate in Hz

// Function to calculate the energy of a window of samples
uint32_t calculate_energy(uint32_t *samples, int32_t size) {
    uint32_t energy = 0.0;
    for (int32_t i = 0; i < size; i++) {
        energy += samples[i] * samples[i]; // Square of the sample
    }
    return energy / size; // Average energy
}

// Function to detect beats in the audio samples and calculate tempo
uint32_t detect_tempo(uint32_t *samples, int32_t total_samples, int32_t *beat_indices, int32_t *beat_count) {
    *beat_count = 0;
    uint32_t last_beat_time = 0.0;
    uint32_t tempo_sum = 0.0;

    for (int32_t i = 0; i < total_samples - WINDOW_SIZE; i += WINDOW_SIZE) {
        // Calculate the energy of the current window
        uint32_t energy = calculate_energy(&samples[i], WINDOW_SIZE);

        // Check if energy exceeds the threshold
        if (energy > ENERGY_THRESHOLD) {
            uint32_t current_beat_time = (uint32_t)i / SAMPLE_RATE; // Time of the beat in seconds
            if (*beat_count > 0) {
                uint32_t interval = current_beat_time - last_beat_time; // Interval between beats
                tempo_sum += interval;
            }
            last_beat_time = current_beat_time;

            // Store the index of the detected beat
            if (*beat_count < total_samples / WINDOW_SIZE) {
                beat_indices[*beat_count] = i;
            }
            (*beat_count)++;
        }
    }

    // Calculate the tempo in BPM if beats were detected
    if (*beat_count > 1) {
        uint32_t average_interval = tempo_sum / (*beat_count - 1); // Average interval in seconds
        return 60.0 / average_interval; // Convert to BPM
    }

    return 0.0; // No beats detected
}
