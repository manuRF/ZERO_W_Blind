/*********************************************************************************
 *		Hardware RPI-ZERO W. and some peripheals home-made.
 *		OS 2017-04-10-raspbian-jessie 
 *   	wiringPi 	projects@drogon.net
 *	 	lirc		http://www.lirc.org/
 *		TSL2561 	Dino Ciuffetti <dino@tuxweb.it> and www.adafruit.com/
 *		@author 	manurf@live.com
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
 *	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
 *	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *	Funcionalidad: control de persianas.
 *	Interfaces UI: pulsadores, mando IR, Android. 
 *  Interfaces medida: Iluminacion (TSL2561-I2C), encoder optico, Diodo IR. 
 *	Interface Potencia: TRIAC BT138, DRIVER MOC3041
 ********************** PINES USADOS EN ESTE PROYECTO	****************************
	3.3VDC	01  VDC IR					5VDC	02  VCC SALIDAS
	GPIO02  03  SDA1 I2C					5VDC	04  VCC SALIDAS
	GPIO03  05  SCL1 I2C					0.0VDC	06	GND SALIDAS	
	GPIO04 	07  ENTRADA_IR					GPIO14  08  TXD0
	0.0VDC	06	GND IR						GPIO15  10  RXDO
	GPIO17 	11 	IN_PERSIANA_BAJAR			GPIO18  12  IN_PERSIANA_COUNT-----------
	GPIO27 	13 	IN_PERSIANA_SUBIR 			0.0VDC	14	GND SALIDAS	
	GPIO22 	15  -----------					GPIO23 	16	PERSIANA_SUBIR	
	3.3VDC  17	VDC IN_COUNT				GPIO24  18  PERSIANA_BAJAR
	GPIO10  19  IN_PERSIANA_COUNT?			0.0VDC	20	GND ENTRADAS
	GPIO09  21  IN_PERSIANA_COUNT?			GPIO25	22	-----------
	GPIO11  23  -----------			 		GPIO08	24	-----------
	GND     25  GND IN_COUNT				GPIO07	26  -----------
	ID_SD   27  -----------					ID_SC	28  -----------
   	GPIO05  29  -----------					GND  	30  -----------
   	GPIO06  31  -----------					GPIO12	32  -----------
   	GPIO13  33  -----------					GND 	34  -----------
   	GPIO19  35  -----------					GPIO16	36  -----------
   	GPIO26  37  -----------					GPIO20	38  -----------
   	GND     39  -----------					GPIO21	40  -----------
 *
 **********************************************************************************
 *
 */ 
 
#ifndef _server_h
#define _server_h

#include <wiringPi.h>
#include "ServerSocket.h"
#include "SocketException.h"
#include <lirc/lirc_client.h>
#include "TSL2561.h"
#include "TSL2561.h"
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <sstream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <iostream>
#include <fstream> 

#define DEBOUNCE 			100		//Retardo para estabilizar los pulsadores
#define TIMETRAVEL 			10000	//Tiempo de todo el recorrido para inicializar
#define SWICHTING 			500		//Retardo para conmutacion en la inversion
#define ABIERTA				100		//Totalmente abierta
#define CERRADA				04		//Totalmente cerrada
#define IRDEBOUNCE			400		//Retardo para estabilizar pulsador del mando IR
#define PORT				104
#define	PERSIANA_SUBIR		23
#define PERSIANA_BAJAR 		24
#define IN_PERSIANA_COUNT 	18	//9
#define	IN_PERSIANA_BAJAR 	17	//25
#define IN_PERSIANA_SUBIR 	27	//8


#define LOGFILE	"/var/log/persianas/NET_login.txt" 
#define LOG  cout //logger //cout

using namespace std;

void setupPins();
void cleanUp();
void initInterrupts();
void zeroInit();
void checklimits();
string logTime();
uint32_t getLux(void);
void kb_handler(int s);
PI_THREAD (TimeTravel);
void PersianaBajar(void);
void PersianaSubir(void);
void PersianaCount(void);
void proc_lirc(void);

#endif
