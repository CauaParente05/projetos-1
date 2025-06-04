#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BMP280.h>

// -------------------------
// Definições de pinos e constantes
// -------------------------

// Sensor ultrassônico (ex: HC-SR04)
const int TRIG_PIN = 8;
const int ECHO_PIN = 9;
const long PROXIMITY_THRESHOLD_CM = 50;  // Distância para iniciar/manter o jogo

// LEDs: 6 LEDs (2 para cada cor)
const int RED_LED_1   = 2;
const int RED_LED_2   = 3;
const int BLUE_LED_1  = 4;
const int BLUE_LED_2  = 5;
const int GREEN_LED_1 = 6;
const int GREEN_LED_2 = 7;

// Tempo de jogo e timeout para reação
const unsigned long GAME_LENGTH = 30000;      // 30 segundos de jogo
const unsigned long EVENT_TIMEOUT = 3000;       // 3 segundos para cada evento

// Multiplexador TCA9548A (endereço padrão 0x70)
// Canais para cada sensor BMP280
#define TCA_ADDR 0x70
const uint8_t CHANNEL_RED   = 0;   // Associado à LED vermelha
const uint8_t CHANNEL_BLUE  = 1;   // Associado à LED azul
const uint8_t CHANNEL_GREEN = 2;   // Associado à LED verde

// Threshold para detectar "assopro" (queda de pressão, em Pascals)
const float BLOW_THRESHOLD = 20.0;

// -------------------------
// Objetos de periféricos
// -------------------------

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Instâncias para os sensores BMP280 – um para cada grupo de cor
Adafruit_BMP280 bmpRed;
Adafruit_BMP280 bmpBlue;
Adafruit_BMP280 bmpGreen;

// Baseline de pressão lida de cada sensor (pressão ambiente)
float baselineRed   = 0;
float baselineBlue  = 0;
float baselineGreen = 0;

// Variáveis do jogo
unsigned long gameStartTime = 0;
int score = 0;

// Enum para representar as cores
enum Color { RED, BLUE, GREEN };

// -------------------------
// Funções auxiliares
// -------------------------

// Seleciona o canal do multiplexador TCA9548A
void tcaSelect(uint8_t channel) {
  if (channel > 7) return;
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << channel);
  Wire.endTransmission();  
}

// Lê a distância do sensor ultrassônico (em cm)
long readUltrasonicDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // Timeout ~5 m
  long cm = duration / 58;
  return cm;
}

// Atualiza a tela LCD com a pontuação e o tempo restante
void updateLCD(int points, unsigned long timeLeft) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pontos: ");
  lcd.print(points);
  lcd.setCursor(0, 1);
  lcd.print("Tempo: ");
  lcd.print(timeLeft / 1000);
  lcd.print("s");
}

// Lê a pressão do sensor BMP280 no canal especificado
float readBMPPressure(Adafruit_BMP280 &bmp, uint8_t channel) {
  tcaSelect(channel);
  delay(10);
  return bmp.readPressure();  
}

// Inicializa um sensor BMP280 no canal especificado
bool initBMP(Adafruit_BMP280 &bmp, uint8_t channel) {
  tcaSelect(channel);
  delay(10);
  // Endereço padrão 0x76 (pode ser alterado se necessário)
  return bmp.begin(0x76);
}

// Acende apenas UMA LED, escolhendo aleatoriamente entre as 2 disponíveis para a cor informada.
// Retorna o pino da LED acesa para que possamos desligá-la posteriormente.
int turnOnSingleLED(Color c) {
  int selectedPin = -1;
  if (c == RED) {
    selectedPin = (random(0, 2) == 0) ? RED_LED_1 : RED_LED_2;
  } else if (c == BLUE) {
    selectedPin = (random(0, 2) == 0) ? BLUE_LED_1 : BLUE_LED_2;
  } else if (c == GREEN) {
    selectedPin = (random(0, 2) == 0) ? GREEN_LED_1 : GREEN_LED_2;
  }
  digitalWrite(selectedPin, HIGH);
  return selectedPin;
}

// -------------------------
// Setup
// -------------------------

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  // Inicializa pinos do sensor ultrassônico
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Inicializa os pinos dos LEDs
  pinMode(RED_LED_1, OUTPUT);
  pinMode(RED_LED_2, OUTPUT);
  pinMode(BLUE_LED_1, OUTPUT);
  pinMode(BLUE_LED_2, OUTPUT);
  pinMode(GREEN_LED_1, OUTPUT);
  pinMode(GREEN_LED_2, OUTPUT);
  
  // Garante que os LEDs comecem desligados
  digitalWrite(RED_LED_1, LOW);
  digitalWrite(RED_LED_2, LOW);
  digitalWrite(BLUE_LED_1, LOW);
  digitalWrite(BLUE_LED_2, LOW);
  digitalWrite(GREEN_LED_1, LOW);
  digitalWrite(GREEN_LED_2, LOW);
  
  // Inicializa a tela LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Aproxime-se");
  
  // Inicializa os sensores BMP280 em seus respectivos canais
  bool okRed   = initBMP(bmpRed, CHANNEL_RED);
  bool okBlue  = initBMP(bmpBlue, CHANNEL_BLUE);
  bool okGreen = initBMP(bmpGreen, CHANNEL_GREEN);
  
  if (!okRed || !okBlue || !okGreen) {
    Serial.println("Erro ao inicializar sensores BMP280!");
    lcd.clear();
    lcd.print("Erro BMP280");
    while (1) {}  // Para o programa
  }
  
  // Leitura de referência (baseline) para cada sensor
  baselineRed   = readBMPPressure(bmpRed, CHANNEL_RED);
  baselineBlue  = readBMPPressure(bmpBlue, CHANNEL_BLUE);
  baselineGreen = readBMPPressure(bmpGreen, CHANNEL_GREEN);
  Serial.println("Setup completo.");
  
  // Inicializa a semente para números aleatórios
  randomSeed(analogRead(A0));
}

// -------------------------
// Loop principal
// -------------------------

void loop() {
  // Enquanto ninguém se aproxima, exibe "Aproxime-se"
  long distance = readUltrasonicDistance();
  if (distance > PROXIMITY_THRESHOLD_CM) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Aproxime-se");
    delay(200);
    return;
  }
  
  // Jogador se aproximou: iniciar o jogo
  score = 0;
  gameStartTime = millis();
  Serial.println("Jogo iniciado!");
  
  // Loop do jogo: enquanto o tempo não esgota
  while (millis() - gameStartTime < GAME_LENGTH) {
    // Se o jogador se afastar durante o jogo, encerra
    distance = readUltrasonicDistance();
    if (distance > PROXIMITY_THRESHOLD_CM) {
      Serial.println("Jogador afastou... fim do jogo.");
      break;
    }
    
    // Atualiza a LCD com a pontuação e tempo restante
    unsigned long timeLeft = GAME_LENGTH - (millis() - gameStartTime);
    updateLCD(score, timeLeft);
    
    // Sorteia aleatoriamente uma cor: 0 (vermelho), 1 (azul) ou 2 (verde)
    int colorIndex = random(0, 3);
    Color currentColor = (Color)colorIndex;
    
    // Acende apenas UMA LED para o grupo da cor sorteada
    int activeLED = turnOnSingleLED(currentColor);
    
    unsigned long eventStartTime = millis();
    bool reacted = false;
    
    // Aguarda até EVENT_TIMEOUT por "assopro" na respectiva estação
    while (millis() - eventStartTime < EVENT_TIMEOUT) {
      // Se o jogador se afastar, encerra o evento (e o jogo)
      distance = readUltrasonicDistance();
      if (distance > PROXIMITY_THRESHOLD_CM) {
        Serial.println("Jogador afastou durante o evento.");
        break;
      }
      
      // Lê a pressão do sensor associado à cor atual
      float pressure = 0;
      switch(currentColor) {
        case RED:
          pressure = readBMPPressure(bmpRed, CHANNEL_RED);
          break;
        case BLUE:
          pressure = readBMPPressure(bmpBlue, CHANNEL_BLUE);
          break;
        case GREEN:
          pressure = readBMPPressure(bmpGreen, CHANNEL_GREEN);
          break;
      }
      
      // Verifica se houve queda na pressão (assopro)
      if ((currentColor == RED   && (baselineRed   - pressure > BLOW_THRESHOLD)) ||
          (currentColor == BLUE  && (baselineBlue  - pressure > BLOW_THRESHOLD)) ||
          (currentColor == GREEN && (baselineGreen - pressure > BLOW_THRESHOLD))) {
        reacted = true;
        score++;
        Serial.print("Ponto marcado! Cor: ");
        if (currentColor == RED)   Serial.println("Vermelho");
        if (currentColor == BLUE)  Serial.println("Azul");
        if (currentColor == GREEN) Serial.println("Verde");
        break;
      }
      delay(50);
    }
    
    // Desliga a LED utilizada no evento
    digitalWrite(activeLED, LOW);
    
    // Se o jogador se afastou, encerra o jogo imediatamente
    if (distance > PROXIMITY_THRESHOLD_CM)
      break;
    
    // Atraso entre eventos para que o jogador tenha tempo de reagir
    delay(random(500, 1500));
  }
  
  // Fim do jogo: exibe a pontuação final por 5 segundos
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fim do jogo!");
  lcd.setCursor(0, 1);
  lcd.print("Pontos: ");
  lcd.print(score);
  delay(5000);
  
  // Após exibição dos resultados, reinicia o jogo
  lcd.clear();
  lcd.print("Aproxime-se");
}