
// эффекты
byte thisBrightness;
int thisColor;
boolean strobeFlag;
float k = 0.25;
int val, rndVal, m_fire_color;
byte fireBright;
uint32_t prevTime;
byte policeState = 0;
int minEffTime = 40;

void effectsTick() {
  if (ONflag) {
    if (btnControl) {
      if (btnColorChanged) {
        btnColorChanged = false;
        for (int i = 0; i <= NUM_LEDS; i++)  {
          leds[i] = HEXcolors[btnColor];
        }
        setBandBrightness (thisBrightness);
        FastLED.show();
      }
    } else {

      //      modeNum = 0;
      if (modeNum < 5) {              // статичные режимы
        if (settingsChanged) {        // по флагу


          settingsChanged = false;

          switch (modeNum) {

            case 0:   // RGB
              setRGBband (presetSettings[1], presetSettings[2], presetSettings[3]);
              thisBrightness = presetSettings[0];
              break;
            case 1:   // HSV
              setHSVband(presetSettings[0], presetSettings[1], presetSettings[2]);
              thisBrightness = presetSettings[3];
              break;
            case 2:   // ColorWheel
              colorWheel(presetSettings[1]);
              thisBrightness = presetSettings[0];
              break;
            case 3:   // ColorSet
              for (int i = 0; i <= NUM_LEDS; i++)  {
                leds[i] = colorVals[presetSettings[1]];
              }
              thisBrightness = presetSettings[0];
              break;
            case 4:   // Kelvin
              setKelvinBand (presetSettings[1]);
              thisBrightness = presetSettings[0];
              break;
          }
          setBandBrightness (thisBrightness);
          FastLED.show();
        }
      } else {

        // динамичные режимы
        if (millis() - effTimer >= effPeriod) {   // по таймеру
          effTimer = millis();
          switch (modeNum) {
            case 5:   // ColorWheel
              effPeriod = presetSettings[1];
              if (effPeriod < minEffTime) effPeriod = minEffTime;
              thisColor += presetSettings[2];
              if (thisColor > 1530) thisColor = 0;
              thisBrightness = presetSettings[0];
              colorWheel(thisColor);
              setBandBrightness (thisBrightness);
              FastLED.show();
              break;
            case 6:   // Fire
              effPeriod = 40;
              fireTick();
              break;
            case 7:   // FireManual
              effPeriod = 40;
              fireM_tick();
              break;
            case 8:   // Strobe

              effPeriod = presetSettings[3];
              if (effPeriod < minEffTime) effPeriod = minEffTime;
              strobeFlag = !strobeFlag;
              setHSVband (presetSettings[0], presetSettings[1], presetSettings[2]);

              thisBrightness = 50 * strobeFlag;
              setBandBrightness (thisBrightness);
              FastLED.show();

              break;
            case 9:   // RndStrobe
              effPeriod = random(presetSettings[3]);
              if (effPeriod < minEffTime) effPeriod = minEffTime;
              strobeFlag = !strobeFlag;

              setHSVband (presetSettings[0], presetSettings[1], presetSettings[2]);
              thisBrightness = 50 * strobeFlag;
              setBandBrightness (thisBrightness);
              FastLED.show();
              break;
            case 10:   // Police
              effPeriod = presetSettings[1];
              if (effPeriod < minEffTime) effPeriod = minEffTime;
              
              thisBrightness = presetSettings[0];             
                    
              if (++policeState >= 8) policeState = 0;
             
              switch (policeState) {
                case 0: setRGBband(0, 0, 0);
                  break;
                case 1: setRGBband(255, 0, 0);
                  break;
                case 2: setRGBband(0, 0, 0);
                  break;
                case 3: setRGBband(255, 0, 0);
                  break;
                case 4: setRGBband(0, 0, 0);
                  break;
                case 5: setRGBband(0, 0, 255);
                  break;
                case 6: setRGBband(0, 0, 0);
                  break;
                case 7: setRGBband(0, 0, 255);
                  break;
              }

              setBandBrightness (thisBrightness);
              FastLED.show();
              
              break;

          }
        }
      }
    }
  }
}

void setW(byte val) {
  //analogWrite(PIN_W, (float)thisBrightness / 255 * val);
  //  FastLED.setBrightness((float)thisBrightness / 255 * val);
}

void fireTick() {
  if (millis() - prevTime > presetSettings[1]) {
    rndVal = random(2, 13) * 10;
    prevTime = millis();
  }
  val = val * (1 - SMOOTH_K) + rndVal * SMOOTH_K;

  int min_bright = presetSettings[0];
  int max_bright = presetSettings[2];

  if (min_bright < 3) min_bright = 4;
  if (max_bright < 4) max_bright = 3;

  if (min_bright < max_bright) min_bright = max_bright + 1;


  thisBrightness = map(val, 20, 120, max_bright, min_bright);

  colorWheel(val);
  setBandBrightness (thisBrightness);
  FastLED.show();

}

void fireM_tick() {
  if (millis() - prevTime > presetSettings[2]) {
    rndVal = random(0, 13) * 10;
    prevTime = millis();
  }
  val = presetSettings[1] * (1 - SMOOTH_KD) + rndVal * SMOOTH_KD;

  m_fire_color = presetSettings[1] * (1 - SMOOTH_KD) + rndVal * SMOOTH_KD;
  m_fire_color = map (m_fire_color, 0, 255, 0, 1530);


  int min_bright = presetSettings[0];
  int max_bright = presetSettings[3];

  if (min_bright < 3) min_bright = 4;
  if (max_bright < 4) max_bright = 3;

  if (min_bright < max_bright) min_bright = max_bright + 1;

  thisBrightness = map(rndVal, 0, 120,  max_bright, min_bright);
  //thisBrightness = presetSettings[0];

  colorWheel(m_fire_color);
  setBandBrightness (thisBrightness);
  FastLED.show();
}
