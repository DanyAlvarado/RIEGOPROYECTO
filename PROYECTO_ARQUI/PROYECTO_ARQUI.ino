#include <Wire.h>
#include <LCD_I2C.h>
#include <DHT.h>
#include <SoftwareSerial.h>

#define clk 7
#define dt 6
#define btn 4

LCD_I2C lcd_i2c(0x27, 20, 4); // Dirección I2C: 0x27, 20 columnas, 4 filas

String menu_principal[] = {"Mostrar Mediciones", "Modificar parametros"};
int max_op_menu = sizeof(menu_principal) / sizeof(menu_principal[0]); // Tamaño del array

String parametros[] = {"Humedad", "Lluvia", "Luz", "Temperatura"};
int max_parametros = sizeof(parametros) / sizeof(parametros[0]);

int state_clk_old;
int state_btn_old;
int count = 0;
int parametroActual = 0; // Índice del parámetro actual a modificar
const int pinRelaySuelo = 11;
const int pinRelayLuz = 10;
bool mostrarMediciones = false;
bool modificarParametros = false;
const int pinDHT11 = 3;

#define DHTTYPE DHT11
DHT dht(pinDHT11, DHTTYPE);

int humedadTierraSetPoint = 800; // Variable para almacenar el valor de humedad a modificar
int lluviaSetPoint = 300;        // Variable para almacenar el valor de lluvia a modificar
int luzSetPoint = 500;           // Variable para almacenar el valor de luz a modificar
int tempSetPoint = 30;           // Variable para almacenar el valor de temperatura a modificar

void setup() {
  lcd_i2c.begin();
  lcd_i2c.backlight();
  pinMode(clk, INPUT);
  pinMode(dt, INPUT);
  pinMode(btn, INPUT_PULLUP);
  pinMode(pinRelaySuelo, OUTPUT);
  pinMode(pinRelayLuz, OUTPUT);

  state_clk_old = digitalRead(clk);
  state_btn_old = digitalRead(btn);
  mostrar_menu_principal();
  dht.begin();
}

void loop() {
  int state_btn = digitalRead(btn);

  encoder();

  if (state_btn_old == HIGH && state_btn == LOW) {
    if (mostrarMediciones) {
      mostrarMediciones = false;
      mostrar_menu_principal();
      delay(200); // Añadir un pequeño retraso para evitar rebotes del botón
    } else if (modificarParametros) {
      correr_opcion();
    } else {
      if (count == 0) {
        mostrarMediciones = true;
        modificarParametros = false;
      } else {
        mostrarMediciones = false;
        modificarParametros = true;
        parametroActual = 0; // Iniciar modificando el primer parámetro
      }
    }
  }

  if (mostrarMediciones) {
    mostrar_mediciones();
  } else if (modificarParametros) {
    modificarParametro();
  }

  state_btn_old = state_btn; // Actualizar estado del botón
}

void correr_opcion() {
  if (modificarParametros) {
    parametroActual++;
    if (parametroActual >= max_parametros) {
      modificarParametros = false;
      mostrarMediciones = false;
      mostrar_menu_principal();
      delay(200); // Añadir un pequeño retraso para evitar rebotes del botón
    }
  }
}

void encoder() {
  int state_clk = digitalRead(clk);
  int state_dt = digitalRead(dt);

  if (state_clk != state_clk_old) {
    if (state_clk == LOW) {
      if (state_dt == HIGH) {
        if (modificarParametros) {
          ajustarParametro(1);
        } else {
          count++;
        }
      } else {
        if (modificarParametros) {
          ajustarParametro(-1);
        } else {
          count--;
        }
      }

      if (count < 0) count = max_op_menu - 1;
      if (count >= max_op_menu) count = 0;

      if (!modificarParametros) {
        mostrar_menu_principal();
      } else {
        mostrar_valor_modificado();
      }
    }

    state_clk_old = state_clk;
  }
}

void ajustarParametro(int cambio) {
  switch (parametroActual) {
    case 0:
      humedadTierraSetPoint += cambio;
      if (humedadTierraSetPoint < 0) humedadTierraSetPoint = 0;
      break;
    case 1:
      lluviaSetPoint += cambio;
      if (lluviaSetPoint < 0) lluviaSetPoint = 0;
      break;
    case 2:
      luzSetPoint += cambio;
      if (luzSetPoint < 0) luzSetPoint = 0;
      break;
    case 3:
      tempSetPoint += cambio;
      if (tempSetPoint < 0) tempSetPoint = 0;
      break;
  }
}

void mostrar_menu_principal() {
  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("Menu Principal");
  lcd_i2c.setCursor(0, 2);
  lcd_i2c.print(menu_principal[count]);
}

void mostrar_mediciones() {
  int humedad = analogRead(A1);
  int luz = analogRead(A3);
  float temp = dht.readTemperature();
  int lluvia = analogRead(A2);

  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("HUMEDAD TIERRA: ");
  lcd_i2c.print(humedad);
  lcd_i2c.setCursor(0, 1);
  lcd_i2c.print("TEMP AMBIENTE: ");
  lcd_i2c.print(temp);
  lcd_i2c.setCursor(0, 2);
  lcd_i2c.print("LUZ: ");
  lcd_i2c.print(luz);
  lcd_i2c.setCursor(0, 3);
  lcd_i2c.print("LLUVIA: ");
  lcd_i2c.print(lluvia);

  if (humedad < humedadTierraSetPoint) {
    digitalWrite(pinRelaySuelo, HIGH);
  } else {
    digitalWrite(pinRelaySuelo, LOW);
  }

  if (temp > tempSetPoint) {
    digitalWrite(pinRelaySuelo, HIGH);
  }

  if (luz > luzSetPoint) {
    digitalWrite(pinRelayLuz, HIGH);
  } else {
    digitalWrite(pinRelayLuz, LOW);
  }

  if (lluvia < lluviaSetPoint) {
    digitalWrite(pinRelaySuelo, LOW);
    lcd_i2c.setCursor(0, 4);
    lcd_i2c.println("Se ha detectado lluvia.");
  }

  delay(1000);
}

void modificarParametro() {
  lcd_i2c.clear();
  lcd_i2c.setCursor(0, 0);
  lcd_i2c.print("Modificar ");
  lcd_i2c.print(parametros[parametroActual]);
  mostrar_valor_modificado();
}

void mostrar_valor_modificado() {
  lcd_i2c.setCursor(0, 1);
  switch (parametroActual) {
    case 0:
      lcd_i2c.print(humedadTierraSetPoint);
      break;
    case 1:
      lcd_i2c.print(lluviaSetPoint);
      break;
    case 2:
      lcd_i2c.print(luzSetPoint);
      break;
    case 3:
      lcd_i2c.print(tempSetPoint);
      break;
  }
  lcd_i2c.print("   "); // Limpiar los caracteres sobrantes
  delay(50);
}


