#define N_FUROS 20
#define MAX_RPM 170
#define MIN_RPM 10

#define kp 1
#define kd 0.06
#define ki 1

#define NUM_MEDICOES 5  // Número de medições para calcular a média

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
float media_velocidade = 0;

float medicoes[NUM_MEDICOES];  // Array para armazenar as últimas medições
int indiceMedicao = 0;         // Índice atual do array
bool arrayCompleto = false;    // Indica se o array já está completo

void setup() {
  pinMode(9, OUTPUT);
  Serial.begin(115200);
  pinMode(13, INPUT);

  // Inicialize o array de medições com zeros
  for (int i = 0; i < NUM_MEDICOES; i++) {
    medicoes[i] = 0;
  }
}

void loop() {
  time = millis();
  if (time > last_time) {
    preset = (analogRead(A0) / 1023.0) * (MAX_RPM - MIN_RPM) + MIN_RPM;
    sensor = digitalRead(13);

    if (s_rise == false && sensor == true) {
      if (time > t_rise) {  // Verifica se o tempo atual é maior que o último tempo de subida
        velocidade = 60000.0 / (N_FUROS * (time - t_rise));
        t_rise = time;

        // Adicione a nova leitura ao array de medições
        medicoes[indiceMedicao] = velocidade;
        indiceMedicao = (indiceMedicao + 1) % NUM_MEDICOES;  // Atualize o índice circular

        // Verifique se o array já está completo
        if (indiceMedicao == 0) {
          arrayCompleto = true;
        }

        // Calcule a média das medições
        float soma = 0;
        int count = arrayCompleto ? NUM_MEDICOES : indiceMedicao;
        for (int i = 0; i < count; i++) {
          soma += medicoes[i];
        }
        media_velocidade = soma / count;

        P_error = preset - media_velocidade;
        I_error = I_error + P_error;
        // D_error = P_error - D_error;

        // control = P_error * kp + D_error * kd + I_error * ki;
        control = P_error * kp + I_error * ki;

        if (control < 0)
          control = 0;

        if (control > 255)
          control = 255;
      }
    }
    s_rise = sensor;

    Serial.print("Velocidade:");
    Serial.print(media_velocidade);
    Serial.print(", Preset:");
    Serial.print(preset);
    Serial.print(", Control:");
    Serial.println(control);

    analogWrite(9, 255 - control);
    last_time = time;
  }
}
