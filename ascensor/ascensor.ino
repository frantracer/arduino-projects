
#define encoderPin1 2
#define encoderPin2 3

// pulsadores llamada
#define llamada1 4         //Pulsador llamada planta sotano
const int  llamada2 = 5;         //pulsador llamada planta baja
const int  llamada3 = 6;         //puladador llamada planta primera

// sensores de planta
const int final1 = 7;            //Final carrera plata sotano
const int final2 = 8;            //final carrera planta baja
const int final3 = 9;            //final carrera planta primera

// led indicadores de plantas
const int led1 = 39;             //led indicador de planta sotano
const int led2 = 41;             //led indicador de planta baja
const int led3 = 43;             //led indicador de planta primera

// movimiento motor
const int bajar = 23;            //salida rele bajada motor
const int subir = 25;            //salida rele subida motor
const int parolento = 27;        //salida rele paro lento

// variable para estado anterior del encoder
int valor_encoder = 0;
int estado_encoder_ultimo = 0b00;

// variable para ascenso o descenso
int piso_actual = -1;
int piso_destino = -1;

// boleanos para la asignacion de un numero a cada llamada o final de carrera
// para poder operar
bool vfinal1, vfinal1Ant;
bool vfinal2, vfinal2Ant;
bool vfinal3, vfinal3Ant;
bool vllamada1, vllamada1Ant;
bool vllamada2, vllamada2Ant;
bool vllamada3, vllamada3Ant;

void setup() {
  Serial.begin (9600);
  
  //PIN DE LLAMADAS COMO ENTRADAS
  pinMode(llamada1, INPUT);
  pinMode(llamada2, INPUT);
  pinMode(llamada3, INPUT);

  //PIN DE FINAL DE CARRERA COMO ENTRADAS
  pinMode(final1, INPUT);
  pinMode(final2, INPUT);
  pinMode(final3, INPUT);

  //PINES DE LED INDICADORES DE PLANTAS
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

  //PINES DE MOVIMIENTO DE MOTOR SALIDAS
  pinMode(subir, OUTPUT);
  pinMode(bajar, OUTPUT);
  pinMode(parolento, OUTPUT);

  // Detectar piso actual
  piso_actual = detectar_piso_actual();
  
  // Estado inicial pines de entrada y salida
  switch(piso_actual) {
    case 1:
      digitalWrite(led1, HIGH);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);
      break;
    case 2:
      digitalWrite(led1, LOW);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, LOW);
      break;
    case 3:
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, HIGH);
      break;
    case -1:
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
      digitalWrite(led3, HIGH);
      break;
  }

  digitalWrite(subir, HIGH);
  digitalWrite(bajar, HIGH);
  
  digitalWrite(parolento, HIGH);

  // Activar pines del encoder
  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);
  
  digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
  digitalWrite(encoderPin2, HIGH); //turn pullup resistor on

  attachInterrupt(0, actualizar_encoder, CHANGE);
  attachInterrupt(1, actualizar_encoder, CHANGE);
}

void loop() {

  if(piso_destino == -1) {
    vllamada1 = digitalRead(llamada1);
    if (vllamada1 && vllamada1Ant)                     // asigadores de valor numerico para cada final de llamada
        piso_destino = 1;                               // cambio el nombre a pisodestino
    vllamada1Ant = vllamada1;   
                                   
    vllamada2 = digitalRead(llamada2);
    if (vllamada2 && vllamada2Ant)
        piso_destino = 2;
    vllamada2Ant = vllamada2;
  
    vllamada3 = digitalRead(llamada3);
    if (vllamada3 && vllamada3Ant)
        piso_destino = 3;
    vllamada3Ant = vllamada3;
  }

  switch (piso_destino) {
      case 1:                                   //caso para la llamada desde el sotano
          if (piso_actual > 1)                   //si el piso en el que esta el montacargas es mayor a 1
            digitalWrite (bajar, LOW);         // esto hara bajar al motor
            digitalWrite (led1, HIGH);          //al pulsar el pulsador del sotano el led 1 del sotano se pone intermitente
            //delay(500);                         
            digitalWrite (led1, LOW);
            //delay(500);
            Serial.println(valor_encoder);
          if (digitalRead(final1)) {          //cuando el final de carrera de la planta sotano se active significa que ya esta el montacargas en la planta sotano y detendra el motor
            digitalWrite (subir, HIGH);
            digitalWrite (bajar, HIGH);
            digitalWrite (led1, HIGH);        //el led 1 se pone fijo cuando se activa el final de carrera del sotano 
            digitalWrite (led2, LOW);         // y se apaga el led de la planta que estaba
            digitalWrite (led3, LOW);
            piso_actual = 1;
            piso_destino = -1;
            Serial.println(valor_encoder);
          }         
          break;

    case 2:                                     //caso para la llamada desde la planta baja
          if (piso_actual > 2)                   //si el piso en el que esta el montacargas es mayor a 2
            digitalWrite(bajar, LOW);          //esto hara bajar al motor
          else if (piso_actual < 2)              //si el piso en el que esta el montagargas es menor a 2
            digitalWrite(subir, LOW);          //esto hara subir al motor
            digitalWrite (led2, HIGH);          //al pulsar el pulsador del baja el led 2 de la planta baja se pone intermitente
            //delay(500);
            digitalWrite (led2, LOW);
            //delay(500);
            Serial.println(valor_encoder);
          if (digitalRead(final2)) {          //cuando el final de carrera de la planta baja se active significa que ya esta el montacargas en la planta baja y detendra motor
            digitalWrite(subir, HIGH);
            digitalWrite (bajar, HIGH);
            digitalWrite (led2, HIGH);        //el led 2 se pone fijo cuando se activa el final de carrera de la planta baja 
            digitalWrite (led1, LOW);         // y se apaga el led de la planta que estaba
            digitalWrite (led3, LOW);
            piso_actual = 2;
            piso_destino = -1;
            Serial.println(valor_encoder);
          }
          break;
       
    case 3:                                //caso para la llamada desde la planta primera
          if (piso_actual < 3)              //si el piso en el que esta el montacargas es menor a 3
            digitalWrite(subir, LOW);     //esto hara subir al motor
            digitalWrite (led3, HIGH);     //al pulsar el pulsador de la plata primera el led 3 de la planta primera se pone intermitente
            //delay(500);
            digitalWrite (led3, LOW);
            //delay(500);
            Serial.println(valor_encoder);
          if (digitalRead(final3)) {      //cuando el final de carrera de la planta primera se active significa que ya esta el montacargas en la planta primera y detendra el motor
            digitalWrite(subir, HIGH);
            digitalWrite (bajar, HIGH);
            digitalWrite (led3, HIGH);    //el led 3 se pone fijo cuando se activa el final de carrera de la planta primera 
            digitalWrite (led1, LOW);     // y se apaga el led de la planta que estaba
            digitalWrite (led2, LOW);    
            piso_actual = 3;
            piso_destino = -1;
            Serial.println(valor_encoder);
          }
          break;
      
    }
}

int detectar_piso_actual() {
  int piso = -1;
  
  if(digitalRead(final1))
    piso = 1;
  else if(digitalRead(final2))
    piso = 2;
  else if(digitalRead(final3))
    piso = 3;
  else
    piso = -1;

  return piso;
}

void actualizar_encoder() {
  int MSB = digitalRead(encoderPin1);  //MSB = Bit más significativo
  int LSB = digitalRead(encoderPin2);  //LSB = Bit menos significativo

  int estado_encoder_actual = (MSB << 1) | LSB;  // Convertir valor pines en un estado
  int sum = (estado_encoder_ultimo << 2) | estado_encoder_actual;  // Juntar estado anterior con el actual

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) // El encoder da un paso en sentido horario
    valor_encoder++;
  if(sum == 0b1110 || sum == 0b1000 || sum == 0b0001 || sum == 0b0111) // El encoder da un paso en sentido antihorario
    valor_encoder--;

  estado_encoder_ultimo = estado_encoder_actual; // Guardamos el ultimo estado del encoder
}
 
