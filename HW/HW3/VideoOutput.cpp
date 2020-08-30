//
// Created by Vincent Bode on 02/05/2020.
//

#include "VideoOutput.h"

#define cimg_display 0

#include "lib/cimg.hpp"
#include <iostream>
#include <vector>
#include <functional>

unsigned char frameIslandsBack[MAP_SIZE][MAP_SIZE][NUM_CHANNELS];
unsigned char frameBeachBack[MAP_SIZE][MAP_SIZE][NUM_CHANNELS];
unsigned char frameDeepWaterBack[MAP_SIZE][MAP_SIZE][NUM_CHANNELS];
unsigned char frameWavesBack[MAP_SIZE][MAP_SIZE][NUM_CHANNELS];
cimg_library::CImg<unsigned char> shipImage;
cimg_library::CImg<unsigned char> anchorImage;

// Initialize to zero
unsigned char frame[MAP_SIZE][MAP_SIZE][NUM_CHANNELS] = {{}};

FILE *ffmpeg;

inline void
overlayAtPoint(unsigned char (&frame)[MAP_SIZE][MAP_SIZE][NUM_CHANNELS], float opacityOffset, float opacityScale,
               std::function<unsigned char(int, int, int)> getOverlay,
               std::function<float(int, int)> getAlpha, int x, int y) {
    float alpha = std::clamp(opacityScale * (getAlpha(x, y) - opacityOffset), 0.0f, 1.0f);
    for (int channel = 0; channel < NUM_CHANNELS; channel++) {
        frame[x][y][channel] = (unsigned char) ((alpha * getOverlay(x, y, channel) +
                                                 (1 - alpha) * frame[x][y][channel]));
    }
}

void overlay(unsigned char (&frame)[MAP_SIZE][MAP_SIZE][NUM_CHANNELS], float opacityOffset, float opacityScale,
             std::function<unsigned char(int, int, int)> getOverlay,
             std::function<float(int, int)> getAlpha) {
#pragma omp parallel for
    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {
            overlayAtPoint(frame, opacityOffset, opacityScale, getOverlay, getAlpha, x, y);
        }
    }
}

void overlayTexture(unsigned char (&frame)[MAP_SIZE][MAP_SIZE][NUM_CHANNELS], float opacityOffset, float opacityScale,
                    unsigned char (&overlayTexture)[MAP_SIZE][MAP_SIZE][NUM_CHANNELS],
                    float (&mapData)[MAP_SIZE][MAP_SIZE]) {
    overlay(frame, opacityOffset, opacityScale,
            [&overlayTexture](int x, int y, int c) { return overlayTexture[x][y][c]; },
            [&mapData](int x, int y) { return mapData[x][y]; }
    );
}

void overlayColor(unsigned char (&frame)[MAP_SIZE][MAP_SIZE][NUM_CHANNELS], float opacityOffset, float opacityScale,
                  unsigned char (&overlayColor)[NUM_CHANNELS],
                  float (&mapData)[MAP_SIZE][MAP_SIZE]) {
    overlay(frame, opacityOffset, opacityScale,
            [&overlayColor](int, int, int c) { return overlayColor[c]; },
            [&mapData](int x, int y) { return mapData[x][y]; }
    );
}

void VideoOutput::loadTextures() {
    try {
        shipImage = cimg_library::CImg<unsigned char>("res/sailing-ship.png");
        anchorImage = cimg_library::CImg<unsigned char>("res/anchor.png");

        loadTexture("res/deep_water.png", frameDeepWaterBack);
        loadTexture("res/sand.png", frameBeachBack);
        loadTexture("res/trees.png", frameIslandsBack);
        loadTexture("res/waves.png", frameWavesBack);
    } catch (cimg_library::CImgIOException &exception) {
        std::cerr << "Failed to load textures for visualization. Please ensure that you are launching this program "
                     "from the correct working directory." << std::endl;
        std::cerr << "Reported error was: " << exception.what() << std::endl;
        exit(-1);
    }
}

void VideoOutput::loadTexture(const char *filename, unsigned char target[MAP_SIZE][MAP_SIZE][NUM_CHANNELS]) {
    std::cerr << "Reading texture " << filename << std::endl;
    const cimg_library::CImg<unsigned char> &img = cimg_library::CImg<>(filename);
    if (img.width() != MAP_SIZE || img.height() != MAP_SIZE) {
        fprintf(stderr, "Wrong texture size on file %s: %i, %i\n", filename, img.width(), img.height());
        exit(-1);
    }
    const unsigned char *r = img.data(0, 0, 0, 0);
    const unsigned char *g = img.data(0, 0, 0, 1);
    const unsigned char *b = img.data(0, 0, 0, 2);
    for (int x = 0; x < MAP_SIZE; x++) {
        for (int y = 0; y < MAP_SIZE; y++) {
            target[x][y][0] = r[x * MAP_SIZE + y];
            target[x][y][1] = g[x * MAP_SIZE + y];
            target[x][y][2] = b[x * MAP_SIZE + y];
        }
    }
    std::cerr << "Loaded texture " << filename << std::endl;
}

void drawCIMGImage(const cimg_library::CImg<unsigned char> &image,
                   unsigned char (&target)[MAP_SIZE][MAP_SIZE][NUM_CHANNELS],
                   int xOffset, int yOffset, bool useAlpha) {
    if (image.spectrum() != 4 && useAlpha) {
        throw std::runtime_error(
                "This does not appear to be a RGBA image (" + std::to_string(image.spectrum()) + " channels)");
    }

    int row = image.width();
    int stopX = std::min(image.height(), MAP_SIZE - xOffset);
    int stopY = std::min(image.width(), MAP_SIZE - yOffset);

    const unsigned char *r = image.data(0, 0, 0, 0);
    const unsigned char *g = image.data(0, 0, 0, 1);
    const unsigned char *b = image.data(0, 0, 0, 2);
    const unsigned char *a;
    if (useAlpha) {
        a = image.data(0, 0, 0, NUM_CHANNELS);
    }

    for (int x = 0; x < stopX; x++) {
        int xTarget = x + xOffset;
        for (int y = 0; y < stopY; ++y) {
            int yTarget = y + yOffset;
            float alpha;
            if (useAlpha) {
                alpha = a[x * row + y];
            } else {
                // Assume white is transparent
                alpha = (r[x * row + y] == 255 && g[x * row + y] == 255 && b[x * row + y] == 255) ? 0.0f : 1.0f;
            }
            target[xTarget][yTarget][0] = (unsigned char) ((alpha * r[x * row + y] +
                                                            (1 - alpha) * target[xTarget][yTarget][0]));
            target[xTarget][yTarget][1] = (unsigned char) ((alpha * g[x * row + y] +
                                                            (1 - alpha) * target[xTarget][yTarget][1]));
            target[xTarget][yTarget][2] = (unsigned char) ((alpha * b[x * row + y] +
                                                            (1 - alpha) * target[xTarget][yTarget][2]));
        }
    }
}

void VideoOutput::beginVideoOutput() {

    std::cerr << "Loading textures..." << std::endl;
    loadTextures();

    std::cerr << "Launching ffmpeg... (writing to out.mp4)" << std::endl;
    ffmpeg = popen(
            "ffmpeg -loglevel warning -f rawvideo -s 1024x1024 -r 12 -pix_fmt rgb24 -i - -y -c:v libx264 -preset veryfast -sn out.mp4",
            "w");
    if (!ffmpeg) {
        std::cerr << "Could not launch ffmpeg to encode video output! " << std::endl;
        perror("Error launching ffmpeg");
        exit(-1);
    }
}

void VideoOutput::endVideoOutput() {
    std::cerr << "Waiting for ffmpeg to finish writing video..." << std::endl;
    int returnCode = pclose(ffmpeg);
    if (returnCode != 0) {
        std::cerr << "ffmpeg reported error code: " << returnCode << std::endl;
        exit(-1);
    }
}

void VideoOutput::writeVideoFrames(std::vector<Position2D> &path, ProblemData &problemData) {
    // Deep Water
    overlayTexture(frame, 0.0, 255.0, frameDeepWaterBack, problemData.islandMap);

    // Beach
    overlayTexture(frame, LAND_THRESHOLD, 64.0, frameBeachBack, problemData.islandMap);

    // Draw islands background
    overlayTexture(frame, LAND_THRESHOLD + 0.05f, 64.0, frameIslandsBack, problemData.islandMap);


    // Shallow Water
//        overlayTexture(frame, 0.75, 3.0, shallowWaterBack, islandMap);


    // Waves
    overlayTexture(frame, 0.0, 1.0, frameWavesBack, *problemData.currentWaveIntensity);

    // Search space
    overlay(frame, 0.0, 1.0,
            [](int, int, int c) { return searchSpaceColor[c]; },
            [&problemData](int x, int y) { return (*problemData.currentShipPositions)[x][y] ? 0.5f : 0.0f; }
    );

    // Anchor
    drawCIMGImage(anchorImage, frame, problemData.portRoyal.x - anchorImage.height() / 2,
                  problemData.portRoyal.y - anchorImage.width() / 2, false);

    // Ship
    drawCIMGImage(shipImage, frame, problemData.shipOrigin.x - shipImage.height() / 2,
                  problemData.shipOrigin.y - shipImage.width() / 2,
                  false);

    // Path from ship to Port Royal
    for (auto &position : path) {
        if (position.x < 0 || position.x >= MAP_SIZE || position.y < 0 || position.y >= MAP_SIZE) {
            continue;
        }
        overlayAtPoint(frame, 0.01, 1.0,
                       [](int, int, int c) { return pathColor[c]; },
                       [](int, int) { return 0.5f; },
                       position.x, position.y
        );
    }

//    std::cerr << "Outputting video frame " << t << std::endl;
    fwrite(frame[0][0], sizeof(frame[0][0][0]), NUM_CHANNELS * MAP_SIZE * MAP_SIZE, ffmpeg);
}