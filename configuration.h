/*************************************************************************************/
/*************************************************************************************/
/*                   PROJET IOT / GROUPE AGIR / EISE5 2015-2016                      */
/*                            Fichier de configuration                               */
/*************************************************************************************/
/*************************************************************************************/

/*************************************************************************************/
/*************************************************************************************/
/*                                FONCTION MAIN                                      */
/*************************************************************************************/
/*************************************************************************************/

/*************************************************************************************/
/*************************************************************************************/
/*                              THREAD TEMPERATURE                                   */
/*************************************************************************************/
/*************************************************************************************/

/*************************************************************************************/
/*************************************************************************************/
/*                                   THREAD LED                                      */
/*************************************************************************************/
/*************************************************************************************/

#define PIN_MUX_P0 				P2_13
#define PIN_MUX_P1 				P0_1
#define PIN_MUX_P2 				P0_0
#define LED_ON						1
#define LED_OFF					0
#define TIME_WAIT_BLINK_LED 	500

/*************************************************************************************/
/*************************************************************************************/
/*                               THREAD PRESSION                                     */
/*************************************************************************************/
/*************************************************************************************/

#define PIN_PRESURE_SENSOR_SDA P0_27
#define PIN_PRESURE_SENSOR_SCL P0_28
#define TIME_WAIT_MS_PRESURE_SENSOR 3000

/*************************************************************************************/
/*************************************************************************************/
/*                                THREAD PRESENCE                                    */
/*************************************************************************************/
/*************************************************************************************/

#define TIME_WAIT_MS_INITIALISATION_FAILURE 	20
#define TIME_WAIT_MS_MOUVEMENT_SENSOR 			100

/*************************************************************************************/
/*************************************************************************************/
/*                                   POT & PWM                                       */
/*************************************************************************************/
/*************************************************************************************/

#define PWM_PERIODE_MS 		1
#define PIN_POTENTIOMETRE	P1_31
#define PIN_PWM_LED 			P2_5
#define PWM_VALUE_MIN 		0.05
#define PWM_VALUE_MAX 		0.95
#define PWM_LED_OFF 			0
#define PWM_LED_ON  			1

/*************************************************************************************/
/*************************************************************************************/
/*                                  FIN PROGRAMME                                    */
/*************************************************************************************/
/*************************************************************************************/
