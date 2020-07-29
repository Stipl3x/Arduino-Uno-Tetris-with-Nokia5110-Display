/********************************************************************************
  Software License Agreement (BSD License)

  Copyright (c) 2012, Adafruit Industries
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.
  3. Neither the name of the copyright holders nor the
  names of its contributors may be used to endorse or promote products
  derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  This is a library for our Monochrome Nokia 5110 LCD Displays
  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Limor Fried/Ladyada  for Adafruit Industries.
  BSD license, check license.txt for more information
  All text above, and the splash screen must be included in any redistribution
  **************************************************************************/

/********************************************************************************
  File:         Nokia5110_tetris.ino

  Description:  This file represents the main application for the Tetris
                game. This software makes use of the Adafruit libraries.
                The class Tetris inherits the Adafruit_PCD8544 class.
                This applications was tested using an Arduino Uno Board
                and a Nokia5110 Display.

  Disclaimer:   This software is under open-source license.
                I am not responsible for any damage.
                Contact me for more informations.

  (C) Stipl3x 2020
  **************************************************************************/

#include <time.h>
// Include class library
#include "Tetris.hpp"

// Console properties
constexpr uint8_t WIDTH = 12;
constexpr uint8_t HEIGHT = 20;

// Game objects
constexpr uint8_t SHAPE_WIDTH = 4;
constexpr uint8_t SHAPE_HEIGHT = 4;

// Start position for a shape
uint8_t currentXPostion = WIDTH / 2 - SHAPE_WIDTH / 2;
uint8_t currentYPosition = 1;

// Joystick analog pins
constexpr uint8_t X_PIN = A0;
constexpr uint8_t Y_PIN = A1;

// Game object matrix
uint8_t shapeAsset[7][16] = {
  // L shape
  { 0, 1, 1, 0,
    0, 1, 0, 0,
    0, 1, 0, 0,
    0, 0, 0, 0
  },

  // J shape
  { 0, 1, 1, 0,
    0, 0, 1, 0,
    0, 0, 1, 0,
    0, 0, 0, 0
  },

  // I shape
  { 0, 1, 0, 0,
    0, 1, 0, 0,
    0, 1, 0, 0,
    0, 1, 0, 0
  },

  // Block shape
  { 0, 1, 1, 0,
    0, 1, 1, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
  },

  // T shape
  { 0, 1, 0, 0,
    0, 1, 1, 0,
    0, 1, 0, 0,
    0, 0, 0, 0
  },

  // 4 shape
  { 0, 1, 0, 0,
    0, 1, 1, 0,
    0, 0, 1, 0,
    0, 0, 0, 0
  },

  // IDK shape :D
  { 0, 0, 1, 0,
    0, 1, 1, 0,
    0, 1, 0, 0,
    0, 0, 0, 0
  }
};

Tetris tetris = Tetris(9, 10, 8);

// Game loop functions
bool WantsToStartNewGame();
void StartNewGame();
void RunGame();
void EndGame();

void ProcessInput(uint8_t* t_currentShape);
void RotateShape(uint8_t* t_currentShape);
void CopyShape(uint8_t* t_currentShape, uint8_t* t_shapeToBeCopied);
bool CompareShapes(uint8_t* t_currentShape, uint8_t* t_shapeToBeCompared);
void CheckInitSpaceFor(uint8_t* t_currentShape);

void setup() {
  // TODO: To be fixed the Adafruit logo screen display
  tetris.begin();

  tetris.setContrast(50);

  tetris.setRotation(1);

  tetris.display();

  //delay(1000);
}

void loop() {
  while (WantsToStartNewGame())
  {
    StartNewGame();

    RunGame();

    EndGame();
  }
}

bool WantsToStartNewGame() {
  return true;
}

void StartNewGame() {
  tetris.reset();
  tetris.createGameScreen(WIDTH, HEIGHT);
}

void RunGame() {
  constexpr uint8_t NUMBER_OF_TICKS = 15;

  bool b_isRunning = true;
  bool b_isTimeToUpdate = false;

  uint8_t currentTick = 0;

  // Generate a random seed every new game
  srand(analogRead(0));

  uint8_t randomShapeNumber = rand() % 7;
  uint8_t randomRotator = rand() % 4;
  uint8_t currentShape[16];
  uint8_t futureShape[16];

  // Create the first random shape to use and apply a random rotation
  CopyShape(currentShape, shapeAsset[randomShapeNumber]);
  for (uint8_t rotateIndex = 0; rotateIndex < randomRotator; rotateIndex++)
    RotateShape(currentShape);

  // Create the next random shape to use and apply a random rotation
  randomShapeNumber = rand() % 7;
  CopyShape(futureShape, shapeAsset[randomShapeNumber]);
  for (uint8_t rotateIndex = 0; rotateIndex < randomRotator; rotateIndex++)
    RotateShape(futureShape);

  // Make sure the shape is on the first line
  CheckInitSpaceFor(currentShape);

  tetris.clearDisplay();

  tetris.renderGameScreen();
  tetris.renderGameObject(currentShape, SHAPE_WIDTH, SHAPE_HEIGHT, currentXPostion, currentYPosition);
  tetris.displayFutureGameObject(futureShape, SHAPE_WIDTH, SHAPE_HEIGHT);

  tetris.display();

  while (b_isRunning) {
    if (currentTick++ >= NUMBER_OF_TICKS)
      b_isTimeToUpdate = true;

    ProcessInput(currentShape);

    if (b_isTimeToUpdate) {
      b_isTimeToUpdate = false;
      currentTick = 0;

      // Game can continue
      if (!tetris.isGoingToCollide(currentShape, SHAPE_WIDTH, SHAPE_HEIGHT, currentXPostion, currentYPosition + 1))
        currentYPosition++;
      // Update game and add a new piece
      else {
        // If a new piece was generated and it collides with the board, then end game
        if (tetris.isGoingToCollide(currentShape, SHAPE_WIDTH, SHAPE_HEIGHT, currentXPostion, currentYPosition))
          b_isRunning = false;

        for (uint8_t heightIndex = 0; heightIndex < 4; heightIndex++)
          for (uint8_t widthIndex = 0; widthIndex < 4; widthIndex++)
            tetris.changeAtPosition(currentXPostion + widthIndex, currentYPosition + heightIndex,
                                    currentShape[heightIndex * 4 + widthIndex]);

        tetris.checkForLines();

        // Restart the position
        currentXPostion = WIDTH / 2 - SHAPE_WIDTH / 2;
        currentYPosition = 1;

        // Update the current shape with the next shape
        CopyShape(currentShape, futureShape);

        // Create a new next shape and apply a random rotation
        randomShapeNumber = rand() % 7;
        CopyShape(futureShape, shapeAsset[randomShapeNumber]);
        randomRotator = rand() % 4;
        for (uint8_t rotateIndex = 0; rotateIndex < randomRotator; rotateIndex++)
          RotateShape(futureShape);

        // Make sure it is on first line
        CheckInitSpaceFor(currentShape);
      }

      tetris.clearDisplay();

      tetris.renderGameScreen();
      tetris.renderGameObject(currentShape, SHAPE_WIDTH, SHAPE_HEIGHT, currentXPostion, currentYPosition);
      tetris.displayFutureGameObject(futureShape, SHAPE_WIDTH, SHAPE_HEIGHT);

      tetris.display();
    }

    // Delay for each loop frame
    delay(50);
  }
}
void EndGame() {
  //TODO
}

void ProcessInput(uint8_t* t_currentShape) {
  constexpr uint8_t ROTATE_DELAY = 5;

  bool b_newMove = false;
  static uint8_t rotateCounter = ROTATE_DELAY;

  // Increase the counter
  rotateCounter++;

  uint16_t x_value = analogRead(X_PIN);
  uint16_t y_value = analogRead(Y_PIN);

  // moveDirection = 1 -> left move
  // moveDirection = 2 -> right move
  // moveDirection = 3 -> down move
  // moveDirection = 4 -> rotate
  uint8_t moveDirection = 0;

  if (x_value < 200 && y_value > 300 && y_value < 700)
    moveDirection = 1;
  else if (x_value > 800 && y_value > 300 && y_value < 700)
    moveDirection = 2;
  else if (y_value > 800 && x_value > 300 && x_value < 700)
    moveDirection = 3;
  else if (y_value < 200 && x_value > 300 && x_value < 700)
    moveDirection = 4;

  // Left move
  if (moveDirection == 1) {
    if (!tetris.isGoingToCollide(t_currentShape, SHAPE_WIDTH, SHAPE_HEIGHT, currentXPostion - 1, currentYPosition)) {
      currentXPostion--;
      b_newMove = true;
    }
  }

  // Right move
  else if (moveDirection == 2) {
    if (!tetris.isGoingToCollide(t_currentShape, SHAPE_WIDTH, SHAPE_HEIGHT, currentXPostion + 1, currentYPosition)) {
      currentXPostion++;
      b_newMove = true;
    }
  }

  // Down move
  else if (moveDirection == 3) {
    if (!tetris.isGoingToCollide(t_currentShape, SHAPE_WIDTH, SHAPE_HEIGHT, currentXPostion, currentYPosition + 1)) {
      currentYPosition++;
      b_newMove = true;
    }
  }

  else if (moveDirection == 4) {
    if (rotateCounter >= ROTATE_DELAY) {
      rotateCounter = 0;
      RotateShape(t_currentShape);
      b_newMove = true;
    }
  }

  if (b_newMove) {
    tetris.clearDisplay();
    tetris.renderGameScreen();
    tetris.renderGameObject(t_currentShape, SHAPE_WIDTH, SHAPE_HEIGHT, currentXPostion, currentYPosition);
    tetris.display();
  }
}

void RotateShape(uint8_t* t_currentShape) {
  uint8_t auxShape[16];

  // All cases, but the box
  if (!CompareShapes(t_currentShape, shapeAsset[3])) {
    // Create a copy
    CopyShape(auxShape, t_currentShape);

    // Rotate the copy
    for (uint8_t heightIndex = 0; heightIndex < SHAPE_HEIGHT; heightIndex++)
      for (uint8_t widthIndex = 0; widthIndex < SHAPE_WIDTH; widthIndex++)
        auxShape[heightIndex * SHAPE_HEIGHT + widthIndex] =
          t_currentShape[SHAPE_WIDTH * SHAPE_HEIGHT - (widthIndex + 1) * 4 + heightIndex];

    // If it is ok to rotate, then update the current shape
    if (!tetris.isGoingToCollide(auxShape, SHAPE_WIDTH, SHAPE_HEIGHT, currentXPostion, currentYPosition))
      CopyShape(t_currentShape, auxShape);
  }
}

void CopyShape(uint8_t* t_currentShape, uint8_t* t_shapeToBeCopied) {
  for (uint8_t index = 0; index < SHAPE_WIDTH * SHAPE_HEIGHT; index++)
    t_currentShape[index] = t_shapeToBeCopied[index];
}
bool CompareShapes(uint8_t* t_currentShape, uint8_t* t_shapeToBeCompared) {
  for (uint8_t index = 0; index < SHAPE_WIDTH * SHAPE_HEIGHT; index++) {
    if (t_currentShape[index] != t_shapeToBeCompared[index])
      return false;
  }

  return true;
}

void CheckInitSpaceFor(uint8_t* t_currentShape) {
  while (!tetris.isGoingToCollide(t_currentShape, SHAPE_WIDTH, SHAPE_HEIGHT, currentXPostion, currentYPosition - 1))
    currentYPosition--;
}
