// salidas del encoder
#define PIN_1_ENCODER         2
#define PIN_2_ENCODER         3

// pulsadores llamada
#define PIN_PULSADOR_1        4    // Pulsador llamada planta sotano
#define PIN_PULSADOR_2        5    // Pulsador llamada planta baja
#define PIN_PULSADOR_3        6    // Pulsador llamada planta primera

// sensores de planta
#define PIN_FINAL_CARRERA_1   7    // Final carrera planta sotano
#define PIN_FINAL_CARRERA_2   8    // Final carrera planta baja
#define PIN_FINAL_CARRERA_3   9    // Final carrera planta primera

// led indicadores de plantas
#define PIN_LED_PLANTA_1      39   // Led indicador de planta sotano
#define PIN_LED_PLANTA_2      41   // Led indicador de planta baja
#define PIN_LED_PLANTA_3      43   // Led indicador de planta primera

// movimiento motor
#define PIN_MOTOR_BAJADA      23   // Salida rele bajada motor
#define PIN_MOTOR_SUBIDA      25   // Salida rele subida motor
#define PIN_MOTOR_PARO_LENTO  27   // Salida rele paro lento

// otros valores 
#define TIEMPO_PARPADEO_LED   500  // Duración del parpadeo de los leds en milisegundos

// variables de estado del encoder
int valor_encoder = 0;
int estado_encoder_ultimo = 0b00;

// variable para ascenso o descenso
int piso_actual = -1;
int piso_destino = -1;

// estados de los pulsadores
bool estado_anterior_pulsador_1 = false;
bool estado_anterior_pulsador_2 = false;
bool estado_anterior_pulsador_3 = false;

// estado de los leds
int tiempo_led_ultimo = 0;
bool estado_led_ultimo = false;

void setup() {
  Serial.begin (9600);
  
  // PIN DE LLAMADAS COMO ENTRADAS
  pinMode(PIN_PULSADOR_1, INPUT);
  pinMode(PIN_PULSADOR_2, INPUT);
  pinMode(PIN_PULSADOR_3, INPUT);

  // PIN DE FINAL DE CARRERA COMO ENTRADAS
  pinMode(PIN_FINAL_CARRERA_1, INPUT);
  pinMode(PIN_FINAL_CARRERA_2, INPUT);
  pinMode(PIN_FINAL_CARRERA_3, INPUT);

  // PINES DE LED INDICADORES DE PLANTAS
  pinMode(PIN_LED_PLANTA_1, OUTPUT);
  pinMode(PIN_LED_PLANTA_2, OUTPUT);
  pinMode(PIN_LED_PLANTA_3, OUTPUT);

  // PINES DE MOVIMIENTO DE MOTOR SALIDAS
  pinMode(PIN_MOTOR_BAJADA, OUTPUT);
  pinMode(PIN_MOTOR_SUBIDA, OUTPUT);
  pinMode(PIN_MOTOR_PARO_LENTO, OUTPUT);

  // INICIALIZAR TODOS LOS COMPONENTES DEL SISTEMA

  // Para motor al iniciar
  digitalWrite(PIN_MOTOR_BAJADA, HIGH);
  digitalWrite(PIN_MOTOR_SUBIDA, HIGH);
  digitalWrite(PIN_MOTOR_PARO_LENTO, HIGH);

  // Activar pines del encoder
  pinMode(PIN_1_ENCODER, INPUT_PULLUP);
  pinMode(PIN_2_ENCODER, INPUT_PULLUP);
  
  digitalWrite(PIN_1_ENCODER, HIGH);
  digitalWrite(PIN_2_ENCODER, HIGH);

  attachInterrupt(0, actualizar_encoder, CHANGE);
  attachInterrupt(1, actualizar_encoder, CHANGE);

  // Detectar piso actual
  piso_actual = detectar_piso_actual();
  
  // Estado inicial pines de entrada y salida
  encender_leds(piso_actual, piso_destino);
}

void loop() {

  // Leer pulsadores si no hay un piso de destino
  if(piso_destino == -1) {
    int estado_actual_pulsador_1 = digitalRead(PIN_PULSADOR_1);
    if (!estado_actual_pulsador_1 && estado_anterior_pulsador_1)  // activar botón al levantar el dedo
        piso_destino = 1;                                         // seleccionar el piso de destino al sótano
    estado_anterior_pulsador_1 = estado_actual_pulsador_1;   
                                   
    int estado_actual_pulsador_2 = digitalRead(PIN_PULSADOR_2);
    if (!estado_actual_pulsador_2 && estado_anterior_pulsador_2)  // activar botón al levantar el dedo
        piso_destino = 2;                                         // seleccionar el piso de destino a la planta baja
    estado_anterior_pulsador_2 = estado_actual_pulsador_2;
  
    int estado_actual_pulsador_3 = digitalRead(PIN_PULSADOR_3);  
    if (!estado_actual_pulsador_3 && estado_anterior_pulsador_3)  // activar botón al levantar el dedo
        piso_destino = 3;                                         // seleccionar el piso de destino a la primera planta
    estado_anterior_pulsador_3 = estado_actual_pulsador_3;
  }

  // Si hay un piso de destino, activar el motor hasta que llegue
  switch (piso_destino) {
      case 1:                                   //caso para la llamada desde el sotano
          if (piso_actual > 1)                   //si el piso en el que esta el montacargas es mayor a 1
            digitalWrite (PIN_MOTOR_BAJADA, LOW);         // esto hara bajar al motor
          if (digitalRead(PIN_FINAL_CARRERA_1)) {          //cuando el final de carrera de la planta sotano se active significa que ya esta el montacargas en la planta sotano y detendra el motor
            digitalWrite (PIN_MOTOR_SUBIDA, HIGH);
            digitalWrite (PIN_MOTOR_BAJADA, HIGH);
            piso_actual = 1;
            piso_destino = -1;
            Serial.println(valor_encoder);
          }         
          break;

    case 2:                                     //caso para la llamada desde la planta baja
          if (piso_actual > 2)                   //si el piso en el que esta el montacargas es mayor a 2
            digitalWrite(PIN_MOTOR_BAJADA, LOW);          //esto hara bajar al motor
          else if (piso_actual < 2)              //si el piso en el que esta el montagargas es menor a 2
            digitalWrite(PIN_MOTOR_SUBIDA, LOW);          //esto hara subir al motor
            Serial.println(valor_encoder);
          if (digitalRead(PIN_FINAL_CARRERA_2)) {          //cuando el final de carrera de la planta baja se active significa que ya esta el montacargas en la planta baja y detendra motor
            digitalWrite(PIN_MOTOR_SUBIDA, HIGH);
            digitalWrite (PIN_MOTOR_BAJADA, HIGH);
            piso_actual = 2;
            piso_destino = -1;
            Serial.println(valor_encoder);
          }
          break;
       
    case 3:                                //caso para la llamada desde la planta primera
          if (piso_actual < 3)              //si el piso en el que esta el montacargas es menor a 3
            digitalWrite(PIN_MOTOR_SUBIDA, LOW);     //esto hara subir al motor
            Serial.println(valor_encoder);
          if (digitalRead(PIN_FINAL_CARRERA_3)) {      //cuando el final de carrera de la planta primera se active significa que ya esta el montacargas en la planta primera y detendra el motor
            digitalWrite(PIN_MOTOR_SUBIDA, HIGH);
            digitalWrite (PIN_MOTOR_BAJADA, HIGH);
            piso_actual = 3;
            piso_destino = -1;
            Serial.println(valor_encoder);
          }
          break;
      
    }

    // Encender los leds en función de donde se encuentre el montacargas
    encender_leds(piso_actual, piso_destino);
}

int detectar_piso_actual() {
  int piso = -1;
  
  if(digitalRead(PIN_FINAL_CARRERA_1))
    piso = 1;
  else if(digitalRead(PIN_FINAL_CARRERA_2))
    piso = 2;
  else if(digitalRead(PIN_FINAL_CARRERA_3))
    piso = 3;
  else
    piso = -1;

  return piso;
}

void actualizar_encoder() {
  int MSB = digitalRead(PIN_1_ENCODER);  //MSB = Bit más significativo
  int LSB = digitalRead(PIN_2_ENCODER);  //LSB = Bit menos significativo

  int estado_encoder_actual = (MSB << 1) | LSB;  // Convertir valor pines en un estado
  int sum = (estado_encoder_ultimo << 2) | estado_encoder_actual;  // Juntar estado anterior con el actual

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) // El encoder da un paso en sentido horario
    valor_encoder++;
  if(sum == 0b1110 || sum == 0b1000 || sum == 0b0001 || sum == 0b0111) // El encoder da un paso en sentido antihorario
    valor_encoder--;

  estado_encoder_ultimo = estado_encoder_actual; // Guardamos el ultimo estado del encoder
}
 
void encender_leds(int piso_actual, int piso_destino) {

  // Encender el led de la planta actual
  switch (piso_actual) {
    case 1:
      digitalWrite(PIN_LED_PLANTA_1, HIGH);
      digitalWrite(PIN_LED_PLANTA_2, LOW);
      digitalWrite(PIN_LED_PLANTA_3, LOW);
      break;
    case 2:
      digitalWrite(PIN_LED_PLANTA_1, LOW);
      digitalWrite(PIN_LED_PLANTA_2, HIGH);
      digitalWrite(PIN_LED_PLANTA_3, LOW);
      break;
    case 3:
      digitalWrite(PIN_LED_PLANTA_1, LOW);
      digitalWrite(PIN_LED_PLANTA_2, LOW);
      digitalWrite(PIN_LED_PLANTA_3, HIGH);
      break;
    case -1:
      digitalWrite(PIN_LED_PLANTA_1, HIGH);
      digitalWrite(PIN_LED_PLANTA_2, HIGH);
      digitalWrite(PIN_LED_PLANTA_3, HIGH);
      break;
  }

  // Si el ascensor se está moviendo, el led de la planta destino debe parpadear
  if(piso_destino != -1) {
    // Comprobar si ha pasado el tiempo suficiente para cambiar el estado del led
    int tiempo_led_actual = millis();
    if (tiempo_led_actual - tiempo_led_ultimo > TIEMPO_PARPADEO_LED) {
      // Seleccionar el pin del led que tiene que parpadear
      int pin_led = -1;
      switch (piso_destino) {
        case 1:
          pin_led = PIN_LED_PLANTA_1;
          break;
        case 2:
          pin_led = PIN_LED_PLANTA_2;
          break;
        case 3:
          pin_led = PIN_LED_PLANTA_3;
          break;
      }

      // Si el led estaba encendido hay que apagarlo y viceversa
      if (estado_led_ultimo) {
        digitalWrite(pin_led, LOW);
      } else {
        digitalWrite(pin_led, HIGH);
      }

      estado_led_ultimo = !estado_led_ultimo;
      tiempo_led_ultimo = tiempo_led_actual;
    }
  }
}
