//
// Created by daran on 1/12/2017 to be used in ECE420 Sp17 for the first time.
// Modified by dwang49 on 1/1/2018 to adapt to Android 7.0 and Shield Tablet updates.
//

#include <cmath>
#include "ece420_main.h"

// Student Variables
#define FRAME_SIZE 128
#define SOUND_LENGTH (256000/2)
#define DELAY 20000000

int8_t flag = 0;
int16_t counter = 0;
int16_t output_counter = 0;
int16_t algorithm_frame_counter = 0;
int8_t play_processed_signal = 0;
int8_t output_flag;
float_t ref_signal_original[SOUND_LENGTH] = {};
float_t ref_signal[SOUND_LENGTH] = {};
float_t primary_signal[SOUND_LENGTH] = {};
float_t combined_signal[SOUND_LENGTH] = {};
float_t error_signal[SOUND_LENGTH] = {};

void nLMS();


void ece420ProcessFrame(sample_buf *dataBuf) {
    // Keep in mind, we only have a small amount of time to process each buffer

    // Using {} initializes all values in the array to zero
    int16_t bufferIn[FRAME_SIZE] = {};
    int16_t bufferOut[FRAME_SIZE] = {};



    // Your buffer conversion (unpacking) here
    // Fetch data sample from dataBuf->buf_[], unpack and put into bufferIn[]
    // ******************** START YOUR CODE HERE ******************** //
    for (int i = 0; i < FRAME_SIZE; i++) {
        uint16_t unpacked = 0;
        unpacked += dataBuf->buf_[2*i+1];
        unpacked = unpacked << 8;
        unpacked += dataBuf->buf_[2*i];

        bufferIn[i] = unpacked;
    }

    // Fill original ref signal
    if (flag == 0 && counter < SOUND_LENGTH/FRAME_SIZE) {
        for (int i = 0; i < FRAME_SIZE; i++) {
            ref_signal_original[FRAME_SIZE*counter + i] = (float_t)bufferIn[i];
        }
        counter++;
    }
    else if (flag == 0) {
        flag++;
        counter = 0;
        LOGD("----- Original reference signal filled, input reference signal next -----");
        for (int i = 0; i < DELAY; i++) {
        }
    }

    // Fill ref signal
    if (flag == 1 && counter < SOUND_LENGTH/FRAME_SIZE) {
        for (int i = 0; i < FRAME_SIZE; i++) {
            ref_signal[FRAME_SIZE*counter + i] = (float_t)bufferIn[i];
        }
        counter++;
    }
    else if (flag == 1) {
        flag++;
        counter = 0;
        LOGD("----- Reference signal filled, input primary signal next -----");
        for (int i = 0; i < DELAY; i++) {
        }
    }

    // Fill primary signal
    else if (flag == 2 && counter < SOUND_LENGTH/FRAME_SIZE) {
        for (int i = 0; i < FRAME_SIZE; i++) {
            primary_signal[FRAME_SIZE*counter + i] = (float_t)bufferIn[i];
        }
        counter++;
    }
    else if (flag == 2) {
        flag++;
        counter = 0;
        LOGD("----- Primary signal filled, apply synthesis next -----");
        for (int i = 0; i < DELAY; i++) {
        }
    }

    // Synthesize noise + primary
    else if (flag == 3 && counter < SOUND_LENGTH / FRAME_SIZE) {
        for (int i = 0; i < FRAME_SIZE; i++) {
            combined_signal[FRAME_SIZE*counter + i] = primary_signal[FRAME_SIZE*counter + i] + ref_signal[FRAME_SIZE*counter + i];
        }
        counter++;
    }
    else if (flag == 3) {
        flag++;
        counter = 0;
        LOGD("----- Synthesis signal completed, apply algorithm next -----");
        for (int i = 0; i < DELAY; i++) {
        }
    }

    // Apply algorithm
    else if(flag == 4) {
        if (algorithm_frame_counter > SOUND_LENGTH / FRAME_SIZE) {
            flag ++;
            algorithm_frame_counter = 0;
            LOGD("----- Algorithm finished, output combined signal next -----");
            for (int i = 0; i < DELAY; i++) {
            }
        }
        else {
            nLMS();
        }
    }


    // ********************* END YOUR CODE HERE ********************* //

    // Loop code provided as a suggestion. This loop simulates sample-by-sample processing.
    for (int sampleIdx = 0; sampleIdx < FRAME_SIZE; sampleIdx++) {
        int16_t output = 0;
        // if combined signal filled, write to output, otherwise do not play sound
        if (flag < 5) {
            output_flag = 0;
        }

        else if (play_processed_signal == 0)  {
            output = (int16_t)combined_signal[output_counter*FRAME_SIZE + sampleIdx];
            output_flag = 1;
        }
        else if (play_processed_signal == 1) {
            output = (int16_t)error_signal[output_counter*FRAME_SIZE + sampleIdx];
            output_flag = 1;
        }
        // Grab result and put into bufferOut[]
        bufferOut[sampleIdx] = output;
    }

    if (output_flag) {
        output_counter++;
        if (output_counter > SOUND_LENGTH/FRAME_SIZE) {
            if (play_processed_signal == 0) {
                output_counter = 0;
                play_processed_signal = 1;
                LOGD("----- Combined signal outputted, output processed signal next -----");
                for (int i = 0; i < DELAY; i++) {
                }
            }
            else {
                counter = 0;
                flag = 0;
                output_counter = 0;
                play_processed_signal = 0;
                algorithm_frame_counter = 0;
                LOGD("----- Processed signal outputted, DONE -----");
                for (int i = 0; i < DELAY; i++) {
                }
            }
        }
    }

    // Your buffer conversion (packing) here
    // Fetch data sample from bufferOut[], pack them and put back into dataBuf->buf_[]
    // ******************** START YOUR CODE HERE ******************** //
    for (int i = 0; i < FRAME_SIZE; i++) {
        uint8_t temp_high = (bufferOut[i] >> 8);
        uint8_t temp_low = (bufferOut[i] & 0x00FF);
        dataBuf->buf_[2*i] = temp_low;
        dataBuf->buf_[2*i+1] = temp_high;

    }
    // ********************* END YOUR CODE HERE ********************* //
}

void nLMS() {
    //Apply algorithm to one frame!!

    // ********************* START YOUR CODE HERE ********************* //

    // ********************* END YOUR CODE HERE ********************* //
    algorithm_frame_counter ++;
}

