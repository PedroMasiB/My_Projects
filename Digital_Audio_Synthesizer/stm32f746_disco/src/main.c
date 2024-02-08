#include <filter_coeff.h>
#include "arm_math.h"
#include "FIR_filter.h"
#include "IIR_filter.h"
#include "stm32f7_wm8994_init.h"
#include "stm32f7_display.h"
#include "main.h"
#include "notes.h"
#include "arm_math.h"
#include "tickTimer.h"
#include "signalTables.h"
#include "adsr.h"
#include "oscillator.h"
#include "IIR.h"
#include "LFO.h"


/* Trying to use LCD
#include "stm32f7xx_hal.h"
#include "stm32746g_discovery_lcd.h"
// LCD dimensions
#define LCD_WIDTH  480
#define LCD_HEIGHT 272


// Buffer for the LCD screen
uint32_t lcd_buffer[LCD_WIDTH * LCD_HEIGHT];
*/

#pragma GCC optimize ("O0")
#define SOURCE_FILE_NAME "Titre Graphe"
#define SAMPLE_RATE 44100
#define FE 44100.0


// Define keyboard control changes
#define C1 0x49
#define C2 0x48
#define C3 0x4A
#define C4 0X47
#define C5 0X05
#define C6 0X54
#define C7 0X5D
#define C8 0X0A
//#define C9
//#define C10
//#define C11
#define C12 0X00
#define C13 0X01
#define C14 0X02
#define C15 0X03
#define C16 0X04
//#define C17
//#define C18
//#define C19
//https://www.musicdsp.org/en/latest/Filters/
static void SystemClock_Config(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);
static void usbUserProcess(USBH_HandleTypeDef *pHost, uint8_t vId);
static void midiApplication();
//static void handleMidiMessage(uint8_t *pBuffer);
float midiNoteToFrequency(uint8_t note);


USBH_HandleTypeDef hUSBHost;
static uint8_t midiReceiveBuffer[MIDI_BUF_SIZE];
static __IO uint32_t USBReceiveAvailable = 0;
static AppState appState = APP_IDLE;

extern int16_t rx_sample_L;
extern int16_t rx_sample_R;
extern int16_t tx_sample_L;
extern int16_t tx_sample_R;

state_variable_filter_t IIR_filter;
ADSR envelope;
LFO lfo;
//ADSR envelope_lfo;



Oscillator osc;


float time = 0;
int noteon = 0;





int16_t tablog[1000];

/*
 * hacer los dos modos del adsr (exponencial y lineal, nosotros tenemos el linea. es como se calcula el value del adsr para cada state) :)
 * adsr reset para poder tocar dos notas a la vez -----> hay problemas cuando suelto una nota :)
 * glide para pasar de una frecuencia de una nota a la notra de manera progresiva
 *
 *
 *
 * estabilidad del filtro IIR :)
 * limites de fc1 con LFO :)
 *
 *
 * Arreglar el adsr para poder tocar dos notas a la vez :)
 * LFO para notas
 * suena raro la fc1 cuando la modifico con el lfo conestado
 * normalizar la amplitud de los osciladores (seno, square, sawtooth) --> la amplitud de todos va entre -1 y 1 pero sigue sonando bajo el seno
 * fc1 + adsr->value (+ lfo->output)
 *
 *
 * problemas con el osc.sin y el lfo juntos, hay ruido
 */


void BSP_AUDIO_SAI_Interrupt_CallBack(void)
{

	BSP_LED_On(LED1);
	/* il y a quelques problemes quand on rappel ces fonctiones ensemble parce que le temp d'ejecution
	   c'est tres longue
	   sine_wave(&osc_sine);
	   sawtooth_wave(&osc_saw);
	   square_wave(&osc_sqr);
	*/
	//sawtooth_wave(&osc_saw);
	switch(osc.mode){
		case 0:
			sine_wave(&osc);
			break;
		case 1:
			sawtooth_wave(&osc);
			break;
		case 2:
			square_wave(&osc);
			break;
	}

	if(lfo.on == 0){
		stop_LFO(&lfo);
	}


	// LFO + IIR + ADSR

	update_state(&envelope);
	calculate_envelope_value(&envelope);
	LFO_process(&lfo, &IIR_filter);
	SVF_process(&IIR_filter, osc.sample);
	tx_sample_L = 1000 * envelope.value * IIR_filter.yl_output;
	tx_sample_R = 1000 * envelope.value * IIR_filter.yl_output;


  	BSP_LED_Off(LED1);


  	return;
}


//============================================================================
//		INTERRUPTION CALLBACK ON TIMER PERIOD
//============================================================================
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance==TIM5)
	{

	}
}

int main(void)
{
  	  HAL_Init();
	  MPU_Config();         // configure the MPU attributes as write-through
	  CPU_CACHE_Enable();   // enable the CPU cache
	  SystemClock_Config(); // configure the system clock to 200 Mhz

	  BSP_LED_Init(LED1);   // initialise LED on GPIO pin P   (also accessible on arduino header)
	  BSP_GPIO_Init();      // initialise diagnostic GPIO pin P   (accessible on arduino header)
	  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO); // configure the  blue user pushbutton in GPIO mode
	  BSP_SDRAM_Init();
	  //DAC12_Config();
	  //init_LCD(fs, name, io_method, graph);

	  USBH_Init(&hUSBHost, usbUserProcess, 0);
	  USBH_RegisterClass(&hUSBHost, USBH_MIDI_CLASS);
	  USBH_Start(&hUSBHost);

	  /*
	  // Initialize the LCD screen
		BSP_LCD_Init();
		BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
		BSP_LCD_SelectLayer(0);
		BSP_LCD_Clear(LCD_COLOR_WHITE);

		// Set the font and text color
		BSP_LCD_SetFont(&Font12);
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		*/

	  HAL_Delay(200);


	  OSC_init(&osc, 1.0, 1000.0, 0.0, SAMPLE_RATE, 0.0, 0);
	  SVF_init(&IIR_filter, 800.0, 4.0, 44100.0);
	  adsr_init(&envelope, 0.005, 2.0, 0.8, 0.2, SAMPLE_RATE, 0);
	  //adsr_init(&envelope_lfo, 0.005, 0.25, 0.8, 0.2, SAMPLE_RATE, 0);
	  LFO_init(&lfo, 10.0, 0.0, 44100.0);



	 // tickTimerInit(1);

	  stm32f7_wm8994_init(AUDIO_FREQUENCY_44K,
						  IO_METHOD_INTR,
						  INPUT_DEVICE_INPUT_LINE_1,
						  OUTPUT_DEVICE_HEADPHONE,
						  WM8994_HP_OUT_ANALOG_GAIN_0DB,
						  WM8994_LINE_IN_GAIN_0DB,
						  WM8994_DMIC_GAIN_9DB,
						  SOURCE_FILE_NAME,
						  NOGRAPH);

  while(1)
  {
			
			midiApplication();
			USBH_Process(&hUSBHost);


			/*

			// Draw something on the LCD
			BSP_LCD_Clear(LCD_COLOR_WHITE);
			BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, 0, (uint8_t*)"Hello, LCD!", CENTER_MODE);

			// Update the display
			BSP_LCD_UpdateDisplay();

			*/
  }
}
//=================================================================
void usbUserProcess(USBH_HandleTypeDef *usbHost, uint8_t eventID) {
	UNUSED(usbHost);
	switch (eventID) {
	case HOST_USER_SELECT_CONFIGURATION:
		break;
	case HOST_USER_DISCONNECTION:
		appState = APP_DISCONNECT;
		BSP_LED_Off(LED_GREEN);
		break;
	case HOST_USER_CLASS_ACTIVE:
		appState = APP_READY;
		BSP_LED_On(LED_GREEN);
		break;
	case HOST_USER_CONNECTION:
		appState = APP_START;
		break;
	default:
		break;
	}
}
//=================================================================
void midiApplication(void)
{
	switch (appState) {
	case APP_READY:
		USBH_MIDI_Receive(&hUSBHost, midiReceiveBuffer, MIDI_BUF_SIZE);
		appState = APP_RUNNING;
		break;
	case APP_RUNNING:
		if (USBReceiveAvailable) {
			USBReceiveAvailable = 0;
			USBH_MIDI_Receive(&hUSBHost, midiReceiveBuffer, MIDI_BUF_SIZE);
		}
		break;
	case APP_DISCONNECT:
		appState = APP_IDLE;
		USBH_MIDI_Stop(&hUSBHost);
		break;
	default:
		break;
	}
}

//=================================================================


float midiNoteToFrequency(uint8_t note) {
	return 440.0 * pow(2.0, (note - 69.0) / 12.0);
	/*
	A4 = 440Hz = MIDI note 69
	1 octave = 12 notes
	1 note = 2^(1/12) 
	*/

}


int tab_midi_log[100];

void processMidiPackets() { // handle incoming MIDI packets

	static int i;
	uint8_t *ptr = midiReceiveBuffer;
	uint16_t numPackets = USBH_MIDI_GetLastReceivedDataSize(&hUSBHost)/4; //>> 2;
	

	if (numPackets != 0){
		while (numPackets--)
		{
			ptr++;
			uint32_t type = *ptr++;
			uint32_t subtype = *ptr++;
			uint32_t vel = *ptr++;

			tab_midi_log[i]=type; 
			i=(i+1)%100;
			tab_midi_log[i]=subtype;
			i=(i+1)%100;
			tab_midi_log[i]=vel;
			i=(i+1)%100;

			type = type & 0xF0; // ignore channel
			switch(type)
			{
				case 0x80: // note off
					//note_off(&envelope);
					break;
				case 0x90: // note on
					//note_on(&envelope, subtype);// el problema es que cuando se produce el note off tambien se llama a esta funcion porque
					//tanto el note on como el note off son el case 0x90

					if(vel==0.0){
						note_off(&envelope, subtype);
					}
					else if(vel!=0){
						note_on(&envelope, subtype);
						osc.frequency = midiNoteToFrequency(subtype);
					}

					//osc_sine.frequency = midiNoteToFrequency(subtype);
					//osc_saw.frequency = midiNoteToFrequency(subtype);
					//osc.frequency = midiNoteToFrequency(subtype);
					//IIR_filter.fc = table_freq[subtype]*2; ne fonctionne pas
					//SVF_init(&IIR_filter, table_freq[subtype]*2, 4.0, 44100.0);
					//set_amplitude(&osc_sqr, vel);
					//noteon = 1;
					//adsr_init(&envelope, 10.0, 10.0, table_freq[subtype]*10, 10.0);
					//calculate_envelope_value(&envelope, vel);
					break;
				case 0xA0:
					// polyphonic aftertouch
					break;
				case 0xB0:
					// control change
					switch(subtype)
					{
						case C1: // Controller number C1 0x49
							// set attack time from 5ms and 15s
							adsr_set_attack_time(&envelope, vel);
							set_rate(&envelope);
            				break;
						case C2: // Controller number C2 0x48
							// set decay time from 200ms to 500ms
							adsr_set_decay_time(&envelope, vel);
							set_rate(&envelope);
							break;
						case C3: // Controller number C3 0x4A
							// set sustain level from 0 to 1
							adsr_set_sustain_level(&envelope, vel);
							set_rate(&envelope);
							break;
						case C4: // Controller number C4 0x47
							// set release time from 50ms to 2s
							adsr_set_release_time(&envelope, vel);
							set_rate(&envelope);
							break;
						case C5: // Controller number C5 0x05
							set_cutoff(&IIR_filter, vel);
							//stop_LFO(&lfo);
							//set_cutoff_LFO(&IIR_filter, vel);
							break;
						case C6: // Controller number C6 0x54
							set_Q(&IIR_filter, vel);
							break;
						case C7: // Controller number C7 0x5D
							set_LFO_amplitude(&lfo, vel);
							break;
						case C8: // Controller number C8 0x0A
							set_frequency(&lfo, vel);
							break;

					}
					break;
				case 0xC0:
					// program change
					switch (subtype)
					{
						case C12:
							osc.mode = (osc.mode + 1) %3;
							osc.phase = 0.0;
							break;
						case C13:
							envelope.mode = !envelope.mode;
							break;
						case C14:
							lfo.on = !lfo.on;
							break;
						case C15:
							break;
						case C16:
							break;
					}
					break;
				case 0xD0:
					// channel aftertouch
					break;
				case 0xE0:
					// pitch bend
					break;
				default:
					break;
			}
		}
	}
}
//=================================================================
void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost) {
	//BSP_LED_Toggle(LED_GREEN);
	processMidiPackets();
	USBReceiveAvailable = 1;

}
//=================================================================

/**
  * @brief  Configure the MPU attributes as Write Through for SRAM1/2.
  * @note   The Base Address is 0x20010000 since this memory interface is the AXI.
  *         The Region Size is 256KB, it is related to SRAM1 and SRAM2  memory size.
  * @param  None
  * @retval None
  */
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Configure the MPU attributes as WT for SRAM */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress = 0x20010000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

static void SystemClock_Config(void)
{
  HAL_StatusTypeDef ret = HAL_OK;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  ASSERT(ret != HAL_OK);

  /* activate the OverDrive */
  ret = HAL_PWREx_ActivateOverDrive();
  ASSERT(ret != HAL_OK);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
  ASSERT(ret != HAL_OK);
}
