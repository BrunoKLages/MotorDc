#define N_FUROS 20
#define MAX_RPM 140
#define MIN_RPM 40

#define kp 0.5
#define ki 1
#define kd 0.5

#define NUM_MEDICOES 20  // Número de medições para calcular a média

int preset = 0;
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
float media_preset = 0;

float medicoes[NUM_MEDICOES];  // Array para armazenar as últimas medições
int indiceMedicao = 0;         // Índice atual do array
bool arrayCompleto = false;    // Indica se o array já está completo
float potenciometro[NUM_MEDICOES];  // Array para armazenar as últimas medições

int indicePot = 0;         // Índice atual do array
bool potCompleto = false;    // Indica se o array já está completo

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
  if (time > last_time+10) {  // Atualiza a cada 100ms (10 vezes por segundo)

    // Leitura e mapeamento do valor do potenciômetro
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

        potenciometro[indicePot] = preset;
        indicePot = (indicePot + 1) % NUM_MEDICOES;  // Atualize o índice circular

        // Verifique se o array já está completo
        if (indicePot == 0) {
          potCompleto = true;
        }

        float soma1 = 0;
        int count1 = potCompleto ? NUM_MEDICOES : indicePot;
        for (int i = 0; i < count; i++) {
          soma1 += potenciometro[i];
        }
        media_preset = soma1 / count1;

        // Cálculo dos erros PID
        float last_P_error = P_error;                             // Armazena o erro proporcional anterior
        P_error = media_preset - media_velocidade;                      // Erro proporcional atual
        D_error = (P_error - last_P_error) / (time - last_time);  // Erro derivativo
        I_error += P_error / (time - last_time);                  // Erro integral

        // Serial.print("I_error:");
        // Serial.println(I_error);
        // Controlador PID
        control = P_error * kp + D_error * kd + I_error * ki;
        // control = P_error * kp + I_error * ki;


        // Limita o valor do controle ao intervalo PWM permitido (0-255)
        if (control < 0)
          control = 0;
        if (control > 255)
          control = 255;
        last_time = time;
      }
    }
    s_rise = sensor;

    Serial.print("Velocidade:");
    Serial.print(media_velocidade);
    Serial.print(", Preset:");
    Serial.println(media_preset);
    Serial.print(", Control:");
    Serial.println(control);

    analogWrite(9, media_preset + control);  // Controla a velocidade do motor com PWM
  }
}
