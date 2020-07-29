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
  File:         Tetris.hpp

  Description:  This file represents the heading file for the Tetris class
                that containts basic functions for the creation of the
                Tetris game using a Nokia5110 Display based on Adafruit
                libraries using an Arduino Uno Board.

  Disclaimer:   This software is under open-source license.
                I am not responsible for any damage.
                Contact me for more informations.

  (C) Stipl3x 2020
  **************************************************************************/

// Libraries included for the use of the display
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// Libraries included for the use of standard data types
#include <stdint.h>
#include <stdbool.h>

#define TETRIS_SPACE 0
#define TETRIS_BLOCK 1

// This class inherits the Adafruit_PCD8544 class and others made by Adafruit
class Tetris : public Adafruit_PCD8544 {
  public:
    Tetris(int8_t DC, int8_t CS, int8_t RST); // Constructor

    //*****Public Variables*****


    //*****Public Methods*****
    // Getters
    uint32_t getMyScore() const;

    // Essential game functions
    void reset();
    void createGameScreen(uint8_t t_width, uint8_t t_height);
    void renderGameScreen();
    void renderGameObject(uint8_t* t_currentGameObject, uint8_t t_width, uint8_t t_height, uint8_t t_currentXPosition, uint8_t t_currentYPosition);
    void displayFutureGameObject(uint8_t* t_futureGameObject, uint8_t t_width, uint8_t t_height);

    // Game checkers
    bool isBorder(uint8_t t_widthIndex, uint8_t t_heightIndex) const;
    bool isGoingToCollide(uint8_t* t_currentGameObject, uint8_t t_width, uint8_t t_height, uint8_t t_currentXPosition, uint8_t t_currentYPosition) const;

    // Game instance modifiers
    void changeAtPosition(uint8_t t_widthIndex, uint8_t t_heightIndex, uint8_t t_newFont);
    void checkForLines();
    void updateGameBoard(uint8_t t_lineNumber);

    // Screen functions
    void drawBlock(int16_t xPos, int16_t yPos);



    //*****Only hidden class stuff*****
  private:
    //*****Private Variables*****
    // Game properties
    uint8_t m_width;
    uint8_t m_height;

    uint8_t m_firstColumn;
    uint8_t m_lastColumn;
    uint8_t m_firstLine;
    uint8_t m_lastLine;

    // Game instance
    uint8_t* m_gameInstance;

    // Game components
    uint32_t m_score;

    //*****Private Methods*****
    void this_displayScore();
    void this_displayLogo();
};
