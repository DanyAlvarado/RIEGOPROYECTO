#include <Wire.h>
#include <LCD_I2C.h>
#include <DHT.h>
#define clk 7
#define dt 6
#define btn 4

LCD_I2C lcd_i2c(0x27, 20, 4); // Dirección I2C: 0x27, 20 columnas, 4 filas

String menu_principal [] = {"Mostrar Mediciones", "Modificar parametros"};
int max_op_menu = sizeof(menu_principal)/sizeof(menu_principal[0]); //Tamaño del array

String menu_med [] = {"Luz", "Lluvia", "Temperatura", "Humedad", "Regresar"};
int state_clk_old;
int state_btn_old;
int count = 0;
const int pinRelaySuelo = 11;
const int pinRelayLuz = 10;
bool mostrarMediciones = false;
const int pinDHT11 = 3;

/*int humedad;
int luz;
int temp;
int lluvia;
*/

#define DHTTYPE DHT11
DHT dht(pinDHT11, DHTTYPE);



void setup(){
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

void loop(){
  int state_btn = digitalRead(btn);
  int state_clk = digitalRead(clk);

  encoder();
  
  
  if (state_btn_old == HIGH && state_btn == LOW) {
    correr_opcion();
  }

  if(mostrarMediciones){
    mostrar_mediciones();
  }  

  state_btn_old = state_btn;  // Actualizar estado del botón

  
}

void correr_opcion() {
  if (count == 0) {
    mostrarMediciones = true;
  }else{
    mostrarMediciones = false;
    mostrar_menu_principal();
  }
}

void encoder() {
  int state_clk = digitalRead(clk);
  int state_dt = digitalRead(dt);

  if (state_clk_old == HIGH && state_clk == LOW) {
    if (state_dt == LOW) {
      count--;
    } else {
      count++;
    }

    if (count < 0) count = max_op_menu - 1;
    if (count > max_op_menu - 1) count = 0;

    mostrar_menu_principal();
  }

  state_clk_old = state_clk;  // Actualizar estado del encoder
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
  //temp = (5.0 * temp * 100.0) / 1024.0;
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
  //El riego se activa cuando la humedad baja de 800
  if(humedad < 800){
    digitalWrite(pinRelaySuelo,HIGH);
  }else{
    digitalWrite(pinRelaySuelo,LOW);
  }
  //El riego se evita cuando la temperatura es mayor a 30
  if(temp > 30.0){
    digitalWrite(pinRelaySuelo, HIGH);
  }

  if(luz > 500){
    digitalWrite(pinRelayLuz, HIGH);
  } else {
    digitalWrite(pinRelayLuz, LOW);
  }

  
  delay(1000);  


  
}
void controlarRele(){
  
}

