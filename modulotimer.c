
// LCD
sbit LCD_RS at RE2_bit;
sbit LCD_EN at RE1_bit;
sbit LCD_D4 at RD4_bit;
sbit LCD_D5 at RD5_bit;
sbit LCD_D6 at RD6_bit;
sbit LCD_D7 at RD7_bit;

sbit LCD_RS_Direction at TRISE2_bit;
sbit LCD_EN_Direction at TRISE1_bit;
sbit LCD_D4_Direction at TRISD4_bit;
sbit LCD_D5_Direction at TRISD5_bit;
sbit LCD_D6_Direction at TRISD6_bit;
sbit LCD_D7_Direction at TRISD7_bit;


unsigned char segundos = 0, minutos = 0, horas = 0;
unsigned char flag_1s = 0;
char texto_lcd[9];

// Alarme
unsigned int adc_valor = 0;
unsigned char alarme_horas = 0;
unsigned char alarme_minutos = 0;
unsigned int total_minutos_alarme = 0;
char texto_alarme[9];
unsigned char ja_enviou_A = 0;

// Controle do Alarme
unsigned char alarme_disparado = 0;
unsigned char alarme_ja_toucou = 0;


void interrupt() {
   if (INTCON.TMR0IF) {
      TMR0H = 0xC2;
      TMR0L = 0xF7;
      INTCON.TMR0IF = 0;

      flag_1s = 1;
      LATB.F7 ^= 1;
   }
}

void formatar_tempo(char *buf, char h, char m, char s) {
   buf[0] = (h / 10) + '0';
   buf[1] = (h % 10) + '0';
   buf[2] = ':';
   buf[3] = (m / 10) + '0';
   buf[4] = (m % 10) + '0';
   buf[5] = ':';
   buf[6] = (s / 10) + '0';
   buf[7] = (s % 10) + '0';
   buf[8] = '\0';
}


void main() {

   //debug

   TRISB.RB2 = 0;
   TRISB.RB3 = 0;
   LATB.RB2  = 0;
   LATB.RB3  = 0;

   ADCON1 = 0x0E;
   ADC_Init();

   TRISB.RB7 = 0;                   // LED indicador tempo
   LATB.F7 = 0;

   TRISB.RB0 = 1;                   // Botão RB0 (Confirmar / Desligar)
   TRISC.RC1 = 0;                   // Buzzer
   LATC.RC1  = 1;                   // Buzzer desligado

   TRISD = 0x00;
   TRISA.RA2 = 0; TRISA.RA3 = 0; TRISA.RA4 = 0; TRISA.RA5 = 0;
   LATA = 0x00;

   Lcd_Init();
   UART1_Init(9600);
   Delay_ms(100);
   Lcd_Cmd(_LCD_CLEAR);
   Lcd_Cmd(_LCD_CURSOR_OFF);
   
   
   Lcd_Out(2, 1, "Ajuste o Alarme");

   while (1) {

      adc_valor = ADC_Read(0);
      total_minutos_alarme = ((unsigned long)adc_valor * 60 / 1023);
      alarme_horas = total_minutos_alarme / 60;
      alarme_minutos = total_minutos_alarme % 60;


      texto_alarme[0] = 'A'; texto_alarme[1] = 'L'; texto_alarme[2] = ':';
      texto_alarme[3] = (alarme_horas / 10) + '0';
      texto_alarme[4] = (alarme_horas % 10) + '0';
      texto_alarme[5] = ':';
      texto_alarme[6] = (alarme_minutos / 10) + '0';
      texto_alarme[7] = (alarme_minutos % 10) + '0';
      texto_alarme[8] = '\0';
      Lcd_Out(1, 1, texto_alarme);



      if (Button(&PORTB, 0, 50, 0)) {
         while (Button(&PORTB, 0, 50, 0));
         Lcd_Cmd(_LCD_CLEAR);
         break; // Quebra o laço e vai para o relógio principal
      }
   }


   // START NO RELÓGIO

   T0CON = 0x86;
   TMR0H = 0xC2;
   TMR0L = 0xF7;

   INTCON.TMR0IF = 0;
   INTCON.TMR0IE = 1;
   INTCON.GIE    = 1;

   while(1) {

      if (flag_1s) {
      flag_1s = 0;
      

      if (horas == alarme_horas && ((alarme_minutos > 0 && minutos == (alarme_minutos - 1)) || (alarme_minutos == 0 && minutos == 59)) && segundos == 0) {
         if (ja_enviou_A == 0) {
            UART1_Write('A');
            ja_enviou_A = 1;
            LATB.RB2 = 1;
            LATB.RB3 = 0;
         }
      }


      if (horas == alarme_horas && minutos == alarme_minutos) {
         ja_enviou_A = 0;
      }


         segundos++;
         if (segundos >= 60) {
            segundos = 0;
            minutos++;
            if (minutos >= 60) {
               minutos = 0;
               horas++;
               if (horas >= 24) {
                  horas = 0;
                  alarme_ja_toucou = 0;
               }
            }
         }

         formatar_tempo(texto_lcd, horas, minutos, segundos);
         Lcd_Out(2, 5, texto_lcd);


         if (horas == alarme_horas && minutos == alarme_minutos && alarme_ja_toucou == 0) {
            alarme_disparado = 1;
         }


         if (minutos != alarme_minutos && alarme_disparado == 1) {
            alarme_disparado = 0;
            alarme_ja_toucou = 1;
            LATC.RC1 = 1;
         }
      }


      Lcd_Out(1, 1, texto_alarme);


      if (alarme_disparado) {
         LATC.RC1 = 0;

         if (Button(&PORTB, 0, 50, 0)) {
            alarme_disparado = 0;
            alarme_ja_toucou = 1;
            LATC.RC1 = 1;
            UART1_Write('S');
            LATB.RB3 = 1;
               LATB.RB2 = 0;
            while (Button(&PORTB, 0, 50, 0));
            
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Out(1, 1, "Bom dia!");

    while(1); // trava o programa aqui
         }
      } else {
         LATC.RC1 = 1;
      }

   }
}