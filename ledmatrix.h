#ifndef __LEDMATRIX_H
#define __LEDMATRIX_H
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "TextAnimation.h"

ArduinoLEDMatrix matrix;

// 100 frames maximum. Compute as maximum length of text you want to print (eg. 20 chars) 
// multiplied by font width (eg. 5 for Font_5x7), so 20 chars * 5 px = 100. If you enter lower
// value (or your text get unexpectedly long), animation will be cut and end soon.
TEXT_ANIMATION_DEFINE(anim, 200)

bool requestNext = false;
String matrix_text = "";

void matrixCallback() {
  // callback is executed in IRQ and should run as fast as possible
  requestNext = true;
}

void leds_setup() {
  matrix.begin();
  matrix.beginDraw();

  matrix.stroke(0xFFFFFFFF);
  matrix.textFont(Font_5x7);
  matrix.textScrollSpeed(120);
  matrix.setCallback(matrixCallback);

  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println("");
  matrix.endTextAnimation(SCROLL_LEFT, anim);
  
  matrix.loadTextAnimationSequence(anim);
  matrix.play();
}


void set_matrix_text(String text){
  matrix_text = text;
}


void leds_show() {
  if (requestNext && matrix_text != "") {
    requestNext = false;

    matrix.beginText(0, 1, 0xFFFFFF);
    matrix.print("  ");
    matrix.println(matrix_text);
    matrix.endTextAnimation(SCROLL_LEFT, anim);
    matrix.loadTextAnimationSequence(anim);
    matrix.play();

  }
}

#endif