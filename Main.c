/*************************************************************************************/
/*************************************************************************************/
/*                   PROJET IOT / GROUPE AGIR / EISE5 2015-2016                      */
/*                             achab.lounes@gmail.com                                */
/*************************************************************************************/
/*************************************************************************************/

#include "mbed.h"
#include "rtos.h"
#include "DHT11.h"
#include "BMP180.h"
#include "SoftPWM.h"
#include "apds9960.h"
#include "configuration.h"

#define SDA P0_27
#define SCL P0_28
#define DHT11_PIN P2_12
#define TIME_LED  500
#define WAIT_TEMPERATURE 3000
#define WAIT_PRESSION    3000
#define WAIT_PRESENCE    200
#define PERIODE_BLE      5000
#define TX P4_28
#define RX P4_29

/*************************************************************************************/
/*************************************************************************************/
/*                           PROTOTYPAGE DE FONCTIONS                                */
/*************************************************************************************/
/*************************************************************************************/ 

void thread_temperature(void const *name);
void thread_led(void const *name); 
void thread_pression(void const *name);  
void thread_presence(void const *name);
void trigger();
void printGesture(int gesture); 
int  getGesture(apds9960 *sensor); 
void bleCallBack(void const *name); 
void potAndPwm();
 
/*************************************************************************************/
/*************************************************************************************/
/*                             VARIABLES GLOBALES                                    */
/*************************************************************************************/
/*************************************************************************************/ 
 
Semaphore two_slots(1);
bool intFlag = false;
Serial bluetooth(P4_28, P4_29);
Ticker timer;
Serial pc(USBTX, USBRX);
apds9960 sensor(P0_27,P0_28);
InterruptIn interrupt(P0_24);
DigitalOut myled(LED1);

typedef struct {
    int  temperature_01;
    int  temperature_02;
    int  humidite;
    long pression;
    int  luminosite;
    int  mouvement;
    } Informations;

Informations inf = {0,0,0,0,0,0};

/*************************************************************************************/
/*************************************************************************************/
/*                                FONCTION MAIN                                      */
/*************************************************************************************/
/*************************************************************************************/
  
int main (void) {
    Thread t1(thread_temperature, (void *)"Thread temperature");
    Thread t2(thread_pression   , (void *)"Thread pression");
    Thread t3(thread_presence   , (void *)"Thread presence");
    Thread t4(thread_led, (void *)"Thread Led");
    
    RtosTimer BleSend(bleCallBack, osTimerPeriodic, (void *)"Ble emission");
    BleSend.start(PERIODE_BLE);
    
    potAndPwm();
}

/*************************************************************************************/
/*************************************************************************************/
/*                              THREAD TEMPERATURE                                   */
/*************************************************************************************/
/*************************************************************************************/

void thread_temperature(void const *name) {
    DHT11 capteur(DHT11_PIN);
    int tmp;
    while (true) {
        printf("%s\n\r", (const char*)name);
        tmp = capteur.readData();
        if (tmp != DHT11::OK) {
            printf("Error! %d\r\n",tmp);
        }
        else {
            inf.temperature_01 = capteur.readTemperature();
            inf.humidite = capteur.readHumidity();
            printf("Temperature: %d, Humidity: %d\r\n", capteur.readTemperature(), capteur.readHumidity());
        }
        Thread::wait(WAIT_TEMPERATURE);
    } 
}

/*************************************************************************************/
/*************************************************************************************/
/*                                   THREAD LED                                      */
/*************************************************************************************/
/*************************************************************************************/

void thread_led(void const *name) {
    DigitalOut tab[] = {P2_13, P0_1, P0_0};
    
    while (true) {
        printf("%s\n\r", (const char*)name);
        
        //printf("000 \n\r");
        tab[0] = 0; tab[1] = 0; tab[2] = 0;
        Thread::wait(TIME_LED );
          
        //printf("100 \n\r");       
        tab[0] = 1; tab[1] = 0; tab[2] = 0;
        Thread::wait(TIME_LED );

        //printf("010 \n\r"); 
        tab[0] = 0; tab[1] = 1; tab[2] = 0;
        Thread::wait(TIME_LED );
 
        //printf("110 \n\r"); 
        tab[0] = 1; tab[1] = 1; tab[2] = 0;

        Thread::wait(TIME_LED );
     
        //printf("001 \n\r"); 
        tab[0] = 0; tab[1] = 0; tab[2] = 1;
        Thread::wait(TIME_LED );
   
        //printf("101 \n\r"); 
        tab[0] = 1; tab[1] = 0; tab[2] = 1;
        Thread::wait(TIME_LED );
          
        //printf("011 \n\r"); 
        tab[0] = 0; tab[1] = 1; tab[2] = 1;
        Thread::wait(TIME_LED );
    }
}

/*************************************************************************************/
/*************************************************************************************/
/*                               THREAD PRESSION                                     */
/*************************************************************************************/
/*************************************************************************************/

void thread_pression(void const *name) {
    long temp =0;
    long pressure=0;
    int error =0;
    BMP180 bmp180(SDA, SCL);
    
    while (true) {
        two_slots.wait();
        printf("%s\n\r", (const char*)name);
        error = bmp180.readTP(&temp,&pressure,OVERSAMPLING_ULTRA_HIGH_RESOLUTION);
        
        inf.temperature_02 = temp;
        inf.pression = pressure;
        printf("Temp is %d \r\n",(temp));
        printf("Pressure is %ld\r\n",pressure);
        printf("Error is %d\r\n\r\n",error);
        two_slots.release();
        Thread::wait(WAIT_PRESSION);
    } 
}

/*************************************************************************************/
/*************************************************************************************/
/*                                THREAD PRESENCE                                    */
/*************************************************************************************/
/*************************************************************************************/
  
void trigger() {    
//    pc.printf("triggered\n\r");
    intFlag = true;
}

void printGesture(int gesture) {
    switch ( gesture ) {
        case DIR_UP:
            pc.printf("UP\n\r");
            break;
        case DIR_DOWN:
            pc.printf("DOWN\n\r");
            break;
        case DIR_LEFT:
            pc.printf("LEFT\n\r");
            break;
        case DIR_RIGHT:
            pc.printf("RIGHT\n\r");
            break;
        case DIR_NEAR:
            pc.printf("NEAR\n\r");
            break;
        case DIR_FAR:
            pc.printf("FAR\n\r");
            break;
        default:
            pc.printf("NONE\n\r");
    }
}

int getGesture() {

    if(sensor.isGestureAvailable()) {
        pc.printf("Gesture Available!\n\r");
        // Process it.
        
        switch ( sensor.readGesture() ) {
            case DIR_UP   :
                    inf.mouvement = 5; 
                    return 5;
            case DIR_DOWN : 
                    inf.mouvement = 4; 
                    return 4;
            case DIR_LEFT : 
                    inf.mouvement = 3; 
                    return 3;
            case DIR_RIGHT: 
                    inf.mouvement = 2; 
                    return 2;
            case DIR_NEAR : 
                    inf.mouvement = 6; 
                    return 6;
            case DIR_FAR  : 
                    inf.mouvement = 7; 
                    return 7;
            default: 
                    inf.mouvement = 1; 
                    return 1;
        }
    }
    return DIR_NONE;
    //return 0;
}
  
void thread_presence(void const *name) {
    
    printf("%s\n\r", (const char*)name);
    
    two_slots.wait();
    while(!sensor.ginit(pc)){
        printf("Something went wrong during APDS-9960 init\n\r");
        Thread::wait(20);
    }
    printf("APDS-9960 initialization complete\n\r");
        
    // Start running the APDS-9960 gesture sensor engine    
    while(!sensor.enableGestureSensor(true)){
        printf("Something went wrong during gesture sensor init!\n\r");
        Thread::wait(20);
    }
    printf("Gesture sensor is now running\n\r");
    two_slots.release();
 
    interrupt.fall(&trigger);
           
    while(1) {
        // when interrupt trigerred, flag is set.
        if(intFlag) {
            
            two_slots.wait();
            printGesture(getGesture());
            two_slots.release();
            
            // Clean interrupt handler flag.
            intFlag = false;
        }
            
        // Do somethings else
        wait_ms(100);
    }
}

/*************************************************************************************/
/*************************************************************************************/
/*                                     BLE                                           */
/*************************************************************************************/
/*************************************************************************************/

void bleCallBack(void const *name) {
    printf("%s\n\r", (const char*)name);
    
    printf(" tmp 01      : %3d \n",inf.temperature_01);
    printf(" tmp 02      : %3d \n",inf.temperature_02);
    printf(" humidite    : %3d \n",inf.humidite);
    printf(" pression    : %6ld \n",inf.pression);
    printf(" luminosite  : %3d \n",inf.luminosite);
    printf(" Mouvement   : %1d \n",inf.mouvement);
        
    bluetooth.printf("%3d%3d%3d%6ld%3d%1d"  ,inf.temperature_01
                                            ,inf.humidite
                                            ,inf.temperature_02
                                            ,inf.pression
                                            ,inf.luminosite
                                            ,inf.mouvement);
                                            
    inf.mouvement = 0;
}

/*************************************************************************************/
/*************************************************************************************/
/*                                   POT & PWM                                       */
/*************************************************************************************/
/*************************************************************************************/

void potAndPwm()
{
    AnalogIn pot(PIN_POTENTIOMETRE);
    SoftPWM led = PIN_PWM_LED;
    
    led.period_ms(PWM_PERIODE_MS);
    
    while (true)   {
        if(pot.read() < PWM_VALUE_MIN){
            led = PWM_LED_OFF;
            inf.luminosite = 0;
        }
        else if (pot.read()> PWM_VALUE_MAX){
            led = PWM_LED_ON;
            inf.luminosite = 100;
        }
        else {
            led = pot.read(); 
            inf.luminosite = pot.read()*100;
        }       
    }
}

/*************************************************************************************/
/*************************************************************************************/
/*                                  FIN PROGRAMME                                    */
/*************************************************************************************/
/*************************************************************************************/
