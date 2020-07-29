#pragma once

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
  File:         Tetris.cpp

  Description:  This file represents the source file for the Tetris class
                that containts basic functions for the creation of the
                Tetris game using a Nokia5110 Display based on Adafruit
                libraries using an Arduino Uno Board.

  Disclaimer:   This software is under open-source license.
                I am not responsible for any damage.
                Contact me for more informations.

  (C) Stipl3x 2020
  **************************************************************************/

// Include Tetris class library
#include "Tetris.hpp"

// Constructor that makes use of the Adafruit_PCD8544 constructor with HW SPI
// Write the pin numbers from the board
Tetris::Tetris(int8_t DC, int8_t CS, int8_t RST) : Adafruit_PCD8544(DC, CS, RST) {
  reset();
}

//********************************************************************************

uint32_t Tetris::getMyScore() const {
  return m_score;
}

//********************************************************************************

void Tetris::reset() {
  // Reset all the private variables
  m_width = 0;
  m_height = 0;

  m_firstColumn = 0;
  m_lastColumn = 0;
  m_firstLine = 0;
  m_lastLine = 0;

  // If MyGameInstance was used before, clear it's memory
  if (m_gameInstance != nullptr) {
    delete[] m_gameInstance;
  }
  m_gameInstance = nullptr;

  m_score = 0;

  clearDisplay();
}

void Tetris::createGameScreen(uint8_t t_width, uint8_t t_height) {
  // Update the variables
  m_width = t_width;
  m_height = t_height;

  // Update the indexes for borders
  m_firstColumn = 0;
  m_lastColumn = m_width - 1;
  m_firstLine = 0;
  m_lastLine = m_height - 1;

  // Should be treated as an array
  m_gameInstance = new uint8_t[m_width * m_height];

  // Initialize the game instance with border from the beginning
  for (uint8_t heightIndex = 0; heightIndex < m_height; heightIndex++) {
    for (uint8_t widthIndex = 0; widthIndex < m_width; widthIndex++) {
      if (isBorder(widthIndex, heightIndex)) {
        m_gameInstance[heightIndex * m_width + widthIndex] = TETRIS_BLOCK;
      }
      else {
        m_gameInstance[heightIndex * m_width + widthIndex] = TETRIS_SPACE;
      }
    }
  }
}

void Tetris::renderGameScreen() {
  // Print the playground
  for (uint8_t heightIndex = 0; heightIndex < m_height; heightIndex++)
    for (uint8_t widthIndex = 0; widthIndex < m_width; widthIndex++)
      // Display only where it needs to be data
      if (m_gameInstance[heightIndex * m_width + widthIndex])
        drawBlock(widthIndex, heightIndex);

  // Display score
  this_displayScore();

  // Display the name of the engine creator
  this_displayLogo();
}

void Tetris::renderGameObject(uint8_t* t_currentGameObject, uint8_t t_width, uint8_t t_height, uint8_t t_currentXPosition, uint8_t t_currentYPosition) {
  for (uint8_t heightIndex = 0; heightIndex < t_height; heightIndex++)
    for (uint8_t widthIndex = 0; widthIndex < t_width; widthIndex++)
      // Render only blocks that have font on them
      if (t_currentGameObject[heightIndex * t_height + widthIndex] != TETRIS_SPACE)
        drawBlock(t_currentXPosition + widthIndex, t_currentYPosition + heightIndex);
}

void Tetris::displayFutureGameObject(uint8_t* t_futureGameObject, uint8_t t_width, uint8_t t_height) {
  // TODO
}

//********************************************************************************

bool Tetris::isBorder(uint8_t t_widthIndex, uint8_t t_heightIndex) const {
  if (t_widthIndex == m_firstColumn || t_widthIndex == m_lastColumn ||
      t_heightIndex == m_firstLine || t_heightIndex == m_lastLine) {
    return true; // It is a border
  }

  return false;
}

bool Tetris::isGoingToCollide(uint8_t* t_currentGameObject, uint8_t t_width, uint8_t t_height, uint8_t t_currentXPosition, uint8_t t_currentYPosition) const {
  for (uint8_t heightIndex = 0; heightIndex < t_height; heightIndex++) {
    for (uint8_t widthIndex = 0; widthIndex < t_width; widthIndex++) {
      // Check to see if the space at this position is occupied,
      if (m_gameInstance[(t_currentYPosition + heightIndex) * m_width + (t_currentXPosition + widthIndex)] != TETRIS_SPACE) {
        // and also that the game object is occupied at this position
        if (t_currentGameObject[heightIndex * t_width + widthIndex] != TETRIS_SPACE) {
          return true; // Is going to collide
        }
      }
    }
  }

  return false;
}

//********************************************************************************

void Tetris::changeAtPosition(uint8_t t_widthIndex, uint8_t t_heightIndex, uint8_t t_newFont) {
  // Change only if the "pixel" is not used
  if (m_gameInstance[t_heightIndex * m_width + t_widthIndex] == TETRIS_SPACE) {
    m_gameInstance[t_heightIndex * m_width + t_widthIndex] = t_newFont;
  }
}

void Tetris::checkForLines() {
  bool b_isLine = true;

  // Only inside borders
  for (uint8_t heigthIndex = 1; heigthIndex < m_height - 1; heigthIndex++) {
    b_isLine = true;

    // Go through every line
    for (int widthIndex = 1; widthIndex < m_width - 1; widthIndex++) {
      if (m_gameInstance[heigthIndex * m_width + widthIndex] == TETRIS_SPACE)
        b_isLine = false;
    }

    if (b_isLine)
      updateGameBoard(heigthIndex);
  }
}

void Tetris::updateGameBoard(uint8_t t_lineNumber) {
  // Empty the line with a little animation
  for (uint8_t widthIndex = 1; widthIndex < m_lastColumn; widthIndex++) {
    clearDisplay();
    m_gameInstance[t_lineNumber * m_width + widthIndex] = TETRIS_SPACE;
    renderGameScreen();
    display();
    delay(50);
  }

  // Move the upper pieces down a level for the highest line
  for (uint8_t heightIndex = t_lineNumber; heightIndex > 0; heightIndex--)
    for (uint8_t widthIndex = 1; widthIndex < m_lastColumn; widthIndex++)
      // Take care of the border
      if (isBorder(widthIndex, heightIndex - 1))
        m_gameInstance[heightIndex * m_width + widthIndex] = TETRIS_SPACE;
      else
        m_gameInstance[heightIndex * m_width + widthIndex] = m_gameInstance[(heightIndex - 1) * m_width + widthIndex];

  // Update score
  m_score += 100;

  clearDisplay();
  renderGameScreen();
  display();
}

//********************************************************************************

void Tetris::drawBlock(int16_t xPos, int16_t yPos) {
  xPos *= 4;
  yPos *= 4;

  yPos += 8;

  drawLine(xPos, yPos, xPos + 3, yPos, BLACK);
  drawLine(xPos, yPos + 1, xPos + 3, yPos + 1, BLACK);
  drawLine(xPos, yPos + 2, xPos + 3, yPos + 2, BLACK);
  drawLine(xPos, yPos + 3, xPos + 3, yPos + 3, BLACK);
}

//********************************************************************************
//                                Private methods
//********************************************************************************

void Tetris::this_displayScore() {
  char score[7];
  write("S:");
  String(m_score).toCharArray(score, 7);
  write(score);
}

void Tetris::this_displayLogo() {
  // TODO
}
