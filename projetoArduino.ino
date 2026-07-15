const int leds[] = {9, 10, 11};  
const int pinoLDR = A0;           
const int pinoPotenciometro = A1;  

// LOGS

char comando = 0;
bool amanhecerAtivo = false;
bool modoManual = false;

unsigned long tempoInicio = 0;
unsigned long ultimoLogTempo = 0;


const unsigned long tempoTotalAmanhecer = 60000;


const int limiteEscuro = 400; 

int brilhoVerde, brilhoAmarelo, brilhoVermelho;

void setup() {
  
  Serial.begin(9600);   
  Serial1.begin(9600);  
  
  
  pinMode(leds[0], OUTPUT);
  pinMode(leds[1], OUTPUT);
  pinMode(leds[2], OUTPUT);
  
  
  analogWrite(leds[0], 0);
  analogWrite(leds[1], 0);
  analogWrite(leds[2], 0);

  Serial.println("==================================================");
  Serial.println("--- MONITOR DO AMANHECER ARTIFICIAL SEQUENCIAL ---");
  Serial.println("Arduino Mega Inicializado. Aguardando comando do PIC...");
  Serial.println("==================================================");
}

void loop() {
  
 
  if (Serial1.available() > 0) {
    comando = Serial1.read();
    
   
    //Serial.print("\n[UART] Recebido do PIC: '");
    //Serial.print(comando);
    //Serial.println("'");
    
   
    if (comando == 'A') {

  
      Serial.println("[AÇÃO] Iniciando checagem de luminosidade...");
      modoManual = false; 
      
      int valorLDR = analogRead(pinoLDR);
      //Serial.print("[SENSOR LDR] Leitura atual: ");
      //Serial.println(valorLDR);
      
      if (valorLDR < limiteEscuro) {
        Serial.println("[DECISÃO] Ambiente ESCURO. Iniciando amanhecer de 60 segundos!");
        amanhecerAtivo = true;

            tempoInicio = millis(); 
      } else {
        Serial.println("[DECISÃO] Ambiente CLARO. Ignorando acendimento dos LEDs.");
        amanhecerAtivo = false;

      }
    }

    if (comando == 'S') {
      Serial.println("[AÇÃO] Alarme desligado no PIC.");
      
  
      if (amanhecerAtivo) {
           Serial.println("==================================================");
    Serial.println("--- MONITOR DO AMANHECER ARTIFICIAL SEQUENCIAL ---");
      Serial.println("CONTROLE MANUAL via Potenciômetro");
      Serial.println("==================================================");

        amanhecerAtivo = false; 
        modoManual = true;       
      } else {
   
        Serial.println("[MUDANÇA DE ESTADO] LEDs já estavam apagados. Mantendo desligado.");
 
        modoManual = false;
        analogWrite(leds[0], 0);
        analogWrite(leds[1], 0);
        analogWrite(leds[2], 0);
      }
    }
  }


  if (amanhecerAtivo) {
    unsigned long tempoDecorrido = millis() - tempoInicio;
    

    if (tempoDecorrido > tempoTotalAmanhecer) {
      tempoDecorrido = tempoTotalAmanhecer;
    }

  
    int brilhoTotal = map(tempoDecorrido, 0, tempoTotalAmanhecer, 0, 765);

    brilhoVerde = brilhoTotal;
    if (brilhoVerde > 255) brilhoVerde = 255;
    if (brilhoVerde < 0)   brilhoVerde = 0;

    brilhoAmarelo = brilhoTotal - 255;
    if (brilhoAmarelo > 255) brilhoAmarelo = 255;
    if (brilhoAmarelo < 0)   brilhoAmarelo = 0;

    brilhoVermelho = brilhoTotal - 510;
    if (brilhoVermelho > 255) brilhoVermelho = 255;
    if (brilhoVermelho < 0)   brilhoVermelho = 0;

    analogWrite(leds[0], brilhoVerde);
    analogWrite(leds[1], brilhoAmarelo);
    analogWrite(leds[2], brilhoVermelho);

    // Log
    
  }


  if (modoManual) {
    int valorPot = analogRead(pinoPotenciometro);
    
  
    int brilhoManual = map(valorPot, 0, 1023, 0, 255);
    
    
    analogWrite(leds[0], brilhoManual);
    analogWrite(leds[1], brilhoManual);
    analogWrite(leds[2], brilhoManual);
    
    
    
  }
}