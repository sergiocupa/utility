#include "stringlib.h"
#include "filelib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <shalib.h>
#include <math.h>
#include <stdarg.h>


typedef enum _TypeCode
{
	TYPE_NONE     = 0,
	TYPE_BYTE     = 1,
	TYPE_SHORT    = 2,
	TYPE_INT      = 3,
	TYPE_LONG     = 4,
	TYPE_FLOAT    = 5,
	TYPE_DOUBLE   = 6,
	TYPE_STRING   = 7,
	TYPE_ARRAY    = 8,
	TYPE_STRUCT   = 9,
	TYPE_BOOL     = 10,
	TYPE_DATE     = 11,
	TYPE_TIME     = 12,
	TYPE_DATETIME = 13
}
TypeCode;


typedef struct _LoggerParam
{
	byte  DataType;
	uint  Address;
	uint  Size;
	void* Data;
}
LoggerParam;

#define USB_TX_BUFFER_SIZE 1024
#define PLATFORM_SIZE      32

typedef uint64_t ulong;
typedef int bool;

static int  MAX_BUFFER_OCCUPANCY = (int)((float)USB_TX_BUFFER_SIZE * 0.7f);
static int  buffer_position   = 0;
static int  PARAM_HEADER_SIZE = (sizeof(uint) * 3) + sizeof(ulong);
static byte usb_tx_busy = 0;
static uint counter = 0;
static int  after_counter_leng = 0;

byte usb_tx_buffer[USB_TX_BUFFER_SIZE];

#define BUFFER_SEND_SIZE 100
char buffer_send_format[BUFFER_SEND_SIZE];

static int send_length = 0;
byte usb_tx_temp[USB_TX_BUFFER_SIZE];


void CDC_Transmit_FS(byte *buffer, int length)
{
	//byte* a = malloc(length +1);
	//memcpy(a, buffer, length);
	//a[length] = 0;
	printf("%s", buffer);
	//free(a);

	//usb_tx_busy = 0;
}


void logger_append_debug(const char* format, ...)
{
	for (size_t i = 0; i < buffer_position; ++i)
	{
		printf("%02X ", (unsigned)usb_tx_buffer[i]);
	}
}


void logger_send(ulong counter, const char* format, ...)
{
	after_counter_leng = sprintf_s(buffer_send_format, BUFFER_SEND_SIZE, "0:%lu|", counter);

	va_list ap;
	va_start(ap, format);
	int len = vsnprintf(buffer_send_format + after_counter_leng, BUFFER_SEND_SIZE, format, ap);
	len += after_counter_leng;
	va_end(ap);

	buffer_send_format[len]   = '\n';
	buffer_send_format[len+1] = 0;

	memcpy(usb_tx_buffer + buffer_position, buffer_send_format, len+1);
	buffer_position += len+1;

	if (buffer_position >= MAX_BUFFER_OCCUPANCY)// && !usb_tx_busy)
	{
		send_length = buffer_position;
		memcpy(usb_tx_temp, usb_tx_buffer, send_length);
		buffer_position = 0;
		usb_tx_busy = 1;
		usb_tx_temp[send_length] = 0;
		CDC_Transmit_FS(usb_tx_temp, send_length);
	}
}


void _logger_send(LoggerParam* params, uint param_count)
{
	uint before = buffer_position;
	uint pos = buffer_position + PARAM_HEADER_SIZE;
	int ix = 0;
	while (ix < param_count)//                                                                Protocol: header complement, parameter sizes
	{
		memcpy(usb_tx_buffer + pos, &params[ix].Size, sizeof(uint));
		pos += 4;
		ix++;
	}

	usb_tx_buffer[buffer_position]     = 255;//                                               Protocol: initial header
	usb_tx_buffer[buffer_position + 1] = 254;//                                               Protocol: initial header
	usb_tx_buffer[buffer_position + 2] = 253;//                                               Protocol: initial header
	usb_tx_buffer[buffer_position + 3] = PLATFORM_SIZE;//                                     Protocol: platform size, exemple: 8, 16, 32, 64 bits

	int sz32 = sizeof(uint);
	int sz64 = sizeof(ulong);

	if (counter == UINT64_MAX) counter = 0;
	counter++;

	memcpy(usb_tx_buffer + buffer_position + 4, &pos, sizeof(uint));//         Protocol: Header size     
	memcpy(usb_tx_buffer + buffer_position + 8, &param_count, sizeof(uint));// Protocol: Param count
	memcpy(usb_tx_buffer + buffer_position + 12, &counter, sizeof(ulong));//   Protocol: Package counter

	buffer_position += pos+1;

	ix = 0;
	while (ix < param_count)
	{
		LoggerParam param = params[ix];
		memcpy(usb_tx_buffer + buffer_position, &param.DataType, sizeof(byte));
		buffer_position++;

		memcpy(usb_tx_buffer + buffer_position, &param.Address, sizeof(uint));
		buffer_position += sizeof(uint);

		memcpy(usb_tx_buffer + buffer_position, &param.Size, sizeof(uint));
		buffer_position += sizeof(uint);

		memcpy(usb_tx_buffer + buffer_position, param.Data, param.Size);
		buffer_position += param.Size;

		ix++;
	}

	uint psize = buffer_position - before;

	if ((buffer_position + psize) >= MAX_BUFFER_OCCUPANCY && !usb_tx_busy)
	{
		send_length = buffer_position;
		memcpy(usb_tx_temp, usb_tx_buffer, send_length);
		buffer_position = 0;
		usb_tx_busy = 1;
		CDC_Transmit_FS(usb_tx_temp, send_length);
	}

}



void logger_sins(float va, float vb, float vc)
{
	if (counter == UINT64_MAX) counter = 0;
	counter++;
	logger_send(counter, "%d:%.3f|%d:%.3f|%d:%.3f", 1,va,2,vb,3,vc);
}

#define PWM_PERIOD     4096        // PWM timer period
#define DEAD_TIME      100         // adjustable dead time
#define ADC_RESOLUTION 4096.0f     //
#define Kp             0.05f       // 0.1 PI controller gains
#define Ki             0.005f      // 0.01 PI controller gains
#define PI             3.14159f
#define PI_2           6.28318f
#define SQUARE_3       1.73205f
#define V_REF          3.3f

#define VBUS          1.0f       // tensão de barramento normalizada
#define MOTOR_R       1.0f       // resistência equivalente
#define MOTOR_L       0.01f      // indutância equivalente
#define OMEGA_SLIP    0.2f       // deslizamento fixo (rad/s) para gerar torque
#define K_E           0.1f  
#define ADC_OFFSET    2048.0f



double ADC_SCALE = V_REF / ADC_RESOLUTION;
double theta = 0.0f;
double Ia, Ib, Ic;           // Correntes das fases
double I_alpha, I_beta;      // Correntes no sistema αβ
double I_d, I_q;             // Correntes no sistema dq
double Iq_ref = 1.0f;        // torque reference
double Id_ref = 0.0f;        // torque reference
double int_d = 0, int_q = 0; // PI controller integrals
double V_d, V_q;
double V_alpha, V_beta;
double va, vb, vc;           // Tensões trifásicas
float vmax = 0, vmin = 0, v0 = 0;

static bool USE_ZERO_SEQUENCE = false;

int current_values[2] = { 0,0 };


void simulate_motor_currents(float va, float vb, float vc)
{
	//// 1) Inverse Clarke: fase → αβ
	//double Valpha = va;
	//double Vbeta = (vb - vc) / 1.732050807568;  // sqrt(3)

	//// 2) Gere a back‑EMF em αβ
	//double omega_e = theta;         // velocidade elétrica instantânea
	//double Ealpha = K_E * omega_e * (-sin(theta));
	//double Ebeta = K_E * omega_e * (cos(theta));

	//// 3) Tensão efetiva no estator
	//double Veff_a = Valpha - Ealpha;
	//double Veff_b = Vbeta - Ebeta;

	//// 4) Impedância R + j*ωL
	//double Zr = MOTOR_R;
	//double Zi = MOTOR_L * omega_e;     // sem slip neste modelo
	//double denom = Zr * Zr + Zi * Zi;

	//// 5) Correntes em αβ (Veff · conj(Z) / |Z|²)
	//double Ialpha = (Veff_a * Zr + Veff_b * Zi) / denom;
	//double Ibeta = (Veff_b * Zr - Veff_a * Zi) / denom;

	//// 6) Inverse Clarke para ib
	//double Ia = Ialpha;
	//double Ib = -0.5 * Ialpha + 0.866025403784 * Ibeta;  // √3/2

	//// 7) Converta para “valor ADC” com offset
	//current_values[0] = (int)(Ia / ADC_SCALE + ADC_OFFSET);
	//current_values[1] = (int)(Ib / ADC_SCALE + ADC_OFFSET);

	// 1) Inverse Clarke: fase → αβ
	double Valpha = va;
	double Vbeta = (vb - vc) / 1.732050807568f;  // sqrt(3)

	// 2) Simula impedância j*ωL + R num referencial estacionário:
	//    Iα = Vα / Z, Iβ = Vβ / Z, mas com deslizamento (slip) deslocando o fasor
	double omega_e = theta;               // velocidade elétrica instantânea
	double phi = OMEGA_SLIP;          // slip constante
	double Z_real = MOTOR_R;
	double Z_imag = MOTOR_L * (omega_e + phi);
	double denom = Z_real * Z_real + Z_imag * Z_imag;

	//// Corrente em αβ (multiplicar V por conj(Z)/|Z|^2)
	//float Ialpha = (Valpha * Z_real + Vbeta * Z_imag) / denom;
	//float Ibeta = (Vbeta * Z_real - Valpha * Z_imag) / denom;

	// Simula uma corrente com defasagem e amortecimento
	double Ialpha = (Valpha * MOTOR_R) / (MOTOR_R * MOTOR_R + (MOTOR_L * omega_e) * (MOTOR_L * omega_e));
	double Ibeta = (Vbeta * MOTOR_R) / (MOTOR_R * MOTOR_R + (MOTOR_L * omega_e) * (MOTOR_L * omega_e));

	// 3) Clarke transform inversa: αβ → abc
	//    Ia = Iα
	//    Ib = -0.5·Iα + (√3/2)·Iβ
	double ra = Ialpha;
	double rb = -0.5f * Ialpha + 0.866025403784f * Ibeta;

	current_values[0] = (int)(ra / ADC_SCALE + 2048.0f);
	current_values[1] = (int)(rb / ADC_SCALE + 2048.0f);
}


int main()
{
	int ix = 0;
	while (ix < 1000)
	{
		// 01. Avancar rotor. Se usar encoder ou pulso externo, entao avancar por algumas destas referencias
		theta += 0.1;
		if (theta > PI_2) theta -= PI_2;

		simulate_motor_currents(va, vb, vc);

		// 02. Read currents (Ia and Ib) updated in DMA callback
		Ia = ((double)current_values[0] - 2048) * ADC_SCALE;
		Ib = ((double)current_values[1] - 2048) * ADC_SCALE;

		// 03. Clarke transform
		I_alpha = Ia;
		I_beta = (Ia + 2 * Ib) / SQUARE_3;

		// 04. Park transform
		double c = cosf(theta);// para STM32F407, usar arm_cos_f32
		double s = sinf(theta);// para STM32F407, usar arm_sin_f32
		I_d = c * I_alpha + s * I_beta;
		I_q = -s * I_alpha + c * I_beta;

		// 05. PI controllers
		double err_d = Id_ref - I_d;
		double err_q = Iq_ref - I_q;
		int_d += err_d;
		int_q += err_q;
		V_d = Kp * err_d + Ki * int_d;
		V_q = Kp * err_q + Ki * int_q;

		// 06. Inverse Park
		V_alpha = c * V_d - s * V_q;
		V_beta  = s * V_d + c * V_q;

		// 07. Inverse clarke transform
		va = V_alpha;
		vb = (-V_alpha + SQUARE_3 * V_beta) / 2.0f;
		vc = (-V_alpha - SQUARE_3 * V_beta) / 2.0f;

		if (USE_ZERO_SEQUENCE)
		{
			vmax = fmaxf(fmaxf(va, vb), vc);
			vmin = fminf(fminf(va, vb), vc);
			v0 = -0.5f * (vmax + vmin);
			va += v0;  vb += v0;  vc += v0;
		}



		logger_sins(va, vb, vc);

		//printf("%f|%f|%f\r\n",va,vb,vc);

		//logger_voltages(va, vb, vc);

		// 08. Converter tensões para duty cycles (0 a PWM_PERIOD)
		//TIM1->CCR1 = (va + 1.0) * PWM_PERIOD / 2; // Normalizar de -1 a 1 para 0 a PWM_PERIOD
		//TIM1->CCR2 = (vb + 1.0) * PWM_PERIOD / 2;
		//TIM1->CCR3 = (vc + 1.0) * PWM_PERIOD / 2;
		ix++;
	}




	//const char* file = "E:\\git\\appserver\\x64\\Debug\\api\\service\\img\\solda.jpg";

	//bool exist = file_exists(file);

	//String s1 = { 3,3,"api"};
	//String s2 = { 3,3,"apa" };

	//int eq = string_equals_s(&s1,&s2);


	//const char* test_split1 = "POST /api/usuarios  HTTP/1.1 ";
	//const char* test_split2 = " ";
	//const char* test_split3 = "";
	//int len = strlen(test_split1);

	//StringArray* parts = string_split(test_split1, len, " ", 1, true);

	//if (parts->Count != 3)
	//{
	//	printf("Split com contagem invalida");
	//	getchar();
	//	return 1;
	//}


	//const char* TEST_HEADER = "Content-Type: application/json";

	//if (string_equals_char_range(TEST_HEADER, "Content-", 0, 8))
	//{
	//	if (string_equals_char_range(TEST_HEADER, "Type", 8, 4))
	//	{
	//		int h = 99;
	//	}
	//}

	//const char* utf8_str = "输";

	//size_t leng = 0;
	//byte* data = string_utf8_to_bytes(utf8_str, &leng);

	//
	//int leee = strlen(utf8_str);


	//const char* ABC = "Sergio";
	//char* cc = string_base64_encode(ABC, strlen(ABC));
	//int outl = 0;
	//byte* bb = string_base64_decode(cc,strlen(cc),&outl);

	//

	//unsigned char digest[SHA1_BLOCK_SIZE];
	//sha1((unsigned char*)ABC, strlen(ABC), digest);

	getchar();
	return 0;
}

