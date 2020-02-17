

void colorWheel(int color) {
  int r, g, b;

  if (color <= 255) {                     // красный макс, зелёный растёт
    r = 255;
    g = color;
    b = 0;
  }
  else if (color > 255 && color <= 510) {     // зелёный макс, падает красный
    r = 510 - color;
    g = 255;
    b = 0;
  }
  else if (color > 510 && color <= 765) {     // зелёный макс, растёт синий
    r = 0;
    g = 255;
    b = color - 510;
  }
  else if (color > 765 && color <= 1020) {    // синий макс, падает зелёный
    r = 0;
    g = 1020 - color;
    b = 255;
  }
  else if (color > 1020 && color <= 1275) {   // синий макс, растёт красный
    r = color - 1020;
    g = 0;
    b = 255;
  }
  else if (color > 1275 && color <= 1530) {   // красный макс, падает синий
    r = 255;
    g = 0;
    b = 1530 - color;
  }

  setRGBband(r, g, b);
}


void setRGBband (int r, int g, int b) {
  for (int i = 0; i <= NUM_LEDS; i++)  {
    leds[i].setRGB(r, g, b);
  }
}

void setHSVband (int h, int s, int v) {
  for (int i = 0; i <= NUM_LEDS; i++)  {
    leds[i].setHSV(h, s, v);
  }
}



void setKelvinBand (int16_t temperature) {
  float tmpKelvin, tmpCalc;
  int r, g, b;

  temperature = constrain(temperature, 1000, 40000);
  tmpKelvin = temperature / 100;

  // red
  if (tmpKelvin <= 66) r = 255;
  else {
    tmpCalc = tmpKelvin - 60;
    tmpCalc = (float)pow(tmpCalc, -0.1332047592);
    tmpCalc *= (float)329.698727446;
    tmpCalc = constrain(tmpCalc, 0, 255);
    r = tmpCalc;
  }

  // green
  if (tmpKelvin <= 66) {
    tmpCalc = tmpKelvin;
    tmpCalc = (float)99.4708025861 * log(tmpCalc) - 161.1195681661;
    tmpCalc = constrain(tmpCalc, 0, 255);
    g = tmpCalc;
  } else {
    tmpCalc = tmpKelvin - 60;
    tmpCalc = (float)pow(tmpCalc, -0.0755148492);
    tmpCalc *= (float)288.1221695283;
    tmpCalc = constrain(tmpCalc, 0, 255);
    g = tmpCalc;
  }

  // blue
  if (tmpKelvin >= 66) b = 255;
  else if (tmpKelvin <= 19) b = 0;
  else {
    tmpCalc = tmpKelvin - 10;
    tmpCalc = (float)138.5177312231 * log(tmpCalc) - 305.0447927307;
    tmpCalc = constrain(tmpCalc, 0, 255);
    b = tmpCalc;
  }
  setRGBband(r, g, b);
}

void setBandBrightness (int bright)  {
  if (bright < 3) bright = 3;
  FastLED.setBrightness(bright);
  }

void showPresetSettings ()  {
    Serial.println ("=================================");
    for (int i = 0; i < 10; i++) {
      Serial.print ("presetSettings[");
      Serial.print ( i );
      Serial.print ("] = ");
      Serial.println (presetSettings[i]);
      }
    }  
