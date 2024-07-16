#define N_FUROS 20
#define MAX_RPM 120
#define MIN_RPM 10

#define MAX_VOLTAGE 5
#define MIN_VOLTAGE 2

#define NUM_SAMPLES 3

#define kp 0.008
#define kd 0.06
#define ki 0.015

float preset = 0;
bool sensor = false;
bool s_rise = false;
float velocidade = 0;
float P_error = 0;
float D_error = 0;
float I_error = 0;
int pwm = 0;
unsigned long time = 0;
unsigned long last_time = 0;
unsigned long t_rise = 0;
float control = 0;

void setup() {

  pinMode(9, OUTPUT);
  Serial.begin(115200);
  pinMode(13, INPUT);
}

void loop() {
  time = millis();
  if (time > last_time) {

    preset = (analogRead(A0) / ((float)1023)) * (MAX_RPM - MIN_RPM) + MIN_RPM;

    sensor = digitalRead(13);

    if (s_rise == false && sensor == true) {
      velocidade = 60000 / (float)((N_FUROS * (time - t_rise)));
      t_rise = time;

      P_error = preset - velocidade;
      D_error = P_error - D_error;
      I_error = I_error + P_error;

      control += P_error * kp + D_error * kd + I_error * ki;

      if (control < 0)
        control = 0;

      if (control > 255)
        control = 255;
    }
    s_rise = sensor;

    Serial.print("Velocidade:");
    Serial.print(velocidade);
    Serial.print(",");
    Serial.print("Preset:");
    Serial.print(preset);
    // Serial.print(",");
    // Serial.print("Sensor:");
    // Serial.print(digitalRead(13));
    Serial.print(",");
    Serial.print("control:");
    Serial.println(control);
    // Serial.print("control");
  }

  analogWrite(9, 255 - control);
  last_time = time;
}