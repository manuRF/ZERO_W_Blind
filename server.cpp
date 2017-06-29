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
#include "server.h"

 using namespace std;

 ofstream logger;
 int countDcha = 50;
 int countIzda = 50;

/*
* ***********************************************************************
*/
int main ( void ){
	logger.open (LOGFILE);
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = kb_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	struct lirc_config *config;
	std::ostringstream Lux ;
	LOG <<logTime()<<"INICIANDO SERVER.."<<"PORT:"<<PORT<<endl;
	if (wiringPiSetupGpio () == -1){
		exit (EXIT_FAILURE) ;
	} else {
			setupPins();	
			cleanUp();
	}
	initInterrupts();
	zeroInit();
	if(lirc_init((char*)"lirc",1)==-1)
		{
		LOG<<logTime()<<logTime()<<"Fallo inicializando lirc"<<endl;
		exit(EXIT_FAILURE);
		}
	bool lircdresult = lirc_readconfig(NULL,&config,NULL);
	if(lircdresult==0)		
		LOG<<logTime()<<"Lectura de lircd.conf cargada y OK:"<<lircdresult<<endl;		
 	else LOG<<logTime()<<"fallo en la lectura de lircd.conf"<<lircdresult<<endl;
	pid_t pid_ir;
	pid_ir = fork();
	if(pid_ir < 0)
		LOG<<logTime()<<"ERROR en la creacion de proc_net IR"<<endl;
	// Lanza el proceso LIRC de entrada del DIODO IR
	if(pid_ir == 0)
		{
			proc_lirc();
			exit(0);
		}
  try
    {
      ServerSocket server (PORT);
      while ( true )		  
		{
		ServerSocket new_sock;
		server.accept ( new_sock );
		try
			{
			LOG<<logTime()<<"try 2 Lazo?"<<endl;
			std::string data;
			new_sock >> data;
			LOG<<logTime()<<"COMANDO :"<<data<< endl;
			const char * comando = data.c_str();
			if(strstr(data.c_str(),"PERSIANA_SUBIR")){
			if(countDcha<=ABIERTA)	{
				if(digitalRead(PERSIANA_BAJAR) == 1){
					digitalWrite(PERSIANA_BAJAR,0);
					delay(SWICHTING);		
				}else {
					digitalWrite(PERSIANA_SUBIR,1);
					string subiendo ="SUBIENDO";
					new_sock<<subiendo;
					}
				}else new_sock<<"SUBIENDO COMPLETADA";
			}else if(strstr(data.c_str(),"PERSIANA_BAJAR")){
			if(countDcha>=CERRADA){
				if(digitalRead(PERSIANA_SUBIR) == 1){
				digitalWrite(PERSIANA_SUBIR,0);
				delay(SWICHTING);
				}else {
					digitalWrite(PERSIANA_BAJAR,1);
					string bajando ="BAJANDO";
					new_sock<<bajando;
					}					
				}else new_sock<<"BAJANDO COMPLETADA";
			}if (strstr(data.c_str(),"PERSIANA_PARAR")){
				cleanUp();
				std::ostringstream countDcha_str;
				countDcha_str<<"PARADA AL : "<<countDcha<<"%";
				LOG<<logTime()<<countDcha_str.str()<<endl;
				new_sock<<countDcha_str.str();
			}if (strstr(data.c_str(),"PERSIANAS_STATUS")){
				std::ostringstream status;
				//std::ostringstream Lux ;
				//Lux<<getLux();
				status<<"Persiana Derecha: "<<countDcha<<"%"
					<<"Persiana Izquierda: "<<countIzda<<"%";
				//	<<"Luz: "<<Lux.str()<<" lux";
					new_sock<<status.str();
				}				
			}
		catch ( SocketException& ) {}
		}
    }
  catch ( SocketException& e )
    {
      cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }
  lirc_freeconfig(config);
  return 0;
}
/*
* ***********************************************************************
*/
void checklimits(void){
	if(countDcha >=100){
		digitalWrite(PERSIANA_SUBIR,0);	
		LOG<<logTime()<<"Limite superior :"<<countDcha<<endl;
	}
	if(countDcha <=0){
		digitalWrite(PERSIANA_BAJAR,0);
		LOG<<logTime()<<"Limite inferior :"<<countDcha<<endl;
	}
}	
/*
* ***********************************************************************
*/	
uint32_t getLux(void) {
	int rc;
	uint16_t broadband, ir;
	uint32_t lux=0;
	TSL2561 light1 = TSL2561_INIT(1, TSL2561_ADDR_FLOAT);
	rc = TSL2561_OPEN(&light1);
	if(rc != 0) {
		LOG<<"ERROR INICIALIZANDO TSL2561 :"<<light1.lasterr<<endl;
		TSL2561_CLOSE(&light1);
		return 1;
	}
	rc = TSL2561_SETGAIN(&light1, TSL2561_GAIN_1X);
	rc = TSL2561_SETINTEGRATIONTIME(&light1, TSL2561_INTEGRATIONTIME_101MS);
	rc = TSL2561_SENSELIGHT(&light1, &broadband, &ir, &lux, 1);
	TSL2561_CLOSE(&light1);	
	return (lux);
}
/*
* ************************************************************************************
*/
void zeroInit(){
	LOG<<logTime()<<"Rutina de inicializacion, baja la persiana al 0 "<<endl;
	digitalWrite(PERSIANA_BAJAR, 1);
	delay(TIMETRAVEL);
	digitalWrite(PERSIANA_BAJAR, 0);
	countDcha = 0;
}
/*
* ************************************************************************************
*/
void initInterrupts(){
	if (wiringPiISR (IN_PERSIANA_SUBIR, INT_EDGE_BOTH, &PersianaSubir) < 0){
		LOG<<logTime()<<"Fallo al Planificar interrupcion"<<endl;
		exit (EXIT_FAILURE) ;
		}
	 if (wiringPiISR (IN_PERSIANA_BAJAR, INT_EDGE_BOTH, &PersianaBajar) < 0){
		LOG<<logTime()<<"Fallo al Planificar interrupcion"<<endl;
		exit (EXIT_FAILURE) ;
		}			
	if (wiringPiISR (IN_PERSIANA_COUNT, INT_EDGE_FALLING, &PersianaCount) < 0){
		exit (EXIT_FAILURE) ;
		}
}
/*
* ************************************************************************************
*/
void PersianaSubir(void){
	LOG<<logTime()<<"PersianaSubir CONTADOR : "<<countDcha<<endl;
	LOG<<logTime()<<"PersianaSubir PULSADO EL BOTON DE SUBIR"<<endl;
	//Si esta bajando, primero se para la bajada.
	if(digitalRead(PERSIANA_BAJAR)==1) {
		digitalWrite(PERSIANA_BAJAR,0);	
		LOG<<logTime()<<"Esta bajando, primero se para la bajada"<<endl;
		delay(SWICHTING);
		} 
	delay(DEBOUNCE);
	//Mira si sigue pulsado despues de rebotes.
	if( digitalRead (IN_PERSIANA_SUBIR)==0){
	//Mira si anteriormente estaba subiendo
	bool ESTADO = digitalRead (PERSIANA_SUBIR);		
	//Si anteriormente no estaba subiendo, se activa la subida.
		if(ESTADO==0){
			LOG<<logTime()<<"anteriormente no estaba subiendo, se activa la subida."<<endl;
			digitalWrite(PERSIANA_SUBIR,1);	 
	//Si anteriormente estaba subiendo, se para la subida.
		}else if(ESTADO==1) {
			LOG<<logTime()<<"anteriormente estaba subiendo, se para la subida."<<endl;
			digitalWrite(PERSIANA_SUBIR,0);				
		}
	}
}
/*
* ************************************************************************************
*/
void PersianaBajar(void){
	LOG<<logTime()<<"PersianaBajar PULSADO EL BOTON DE BAJAR"<<endl;
	LOG<<logTime()<<"PersianaBajar CONTADOR : "<<countDcha<<endl;
	//Si esta subiendo, primero se para la subida.
	if(digitalRead(PERSIANA_SUBIR)==1){
		digitalWrite(PERSIANA_SUBIR,0);
		LOG<<logTime()<<"Esta subiendo, primero se para la subida."<<endl;
		delay(SWICHTING);
		}
	delay(DEBOUNCE);
	//Mira si sigue pulsado despues de rebotes.
	if( digitalRead (IN_PERSIANA_BAJAR)==0){
	//Mira si anteriormente estaba bajando.
		bool ESTADO = digitalRead (PERSIANA_BAJAR);
		//Si anteriormente no estaba bajando, se activa la bajada.
		if(ESTADO==0){
				LOG<<logTime()<<"anteriormente no estaba bajando, se activa la bajada."<<endl;
				digitalWrite(PERSIANA_BAJAR,1);
	//Si anteriormente estaba bajando se para la bajada.
		}else if(ESTADO==1){
			LOG<<logTime()<<"anteriormente estaba bajando se para la bajada."<<endl;
			digitalWrite(PERSIANA_BAJAR,0);			
		}	
	}
}
/*
* ************************************************************************************
*/
void proc_lirc(void)
{
	int buttonTimer = millis();
	char *code;
	while(lirc_nextcode(&code)==0){
//If code = NULL, meaning nothing was returned from LIRC socket,
//then skip lines below and start while loop again.		
        if(code==NULL) continue;
		{		
//Make sure there is a 400ms gap before detecting button presses.
        if (millis() - buttonTimer  > IRDEBOUNCE){
			LOG<<logTime()<<"Econtrado  KEY : "<<code<<endl;
			if(strstr (code,"KEY_YELLOW")){
					LOG<<logTime()<<"Econtrado  KEY_YELLOW: SUBE PERSIANA"<<endl;
					if(digitalRead(PERSIANA_BAJAR) == 1){
						digitalWrite(PERSIANA_BAJAR,0);
						delay(SWICHTING);
					}else 
						digitalWrite(PERSIANA_SUBIR,1);
					buttonTimer = millis();
				}				
            else if(strstr (code,"KEY_BLUE")){
					LOG<<logTime()<<"Econtrado  KEY_GREEN: BAJA PERSIANA"<<endl;
					if(digitalRead(PERSIANA_SUBIR) == 1){
						digitalWrite(PERSIANA_SUBIR,0);
						delay(SWICHTING);						
					}else
						digitalWrite(PERSIANA_BAJAR,1);	
					buttonTimer = millis();
				}

			else if (strstr (code,"KEY_INFO")){
					LOG<<logTime()<<"Encontrado key KEY_INFO: PARA PERSIANA"<<endl;
					cleanUp();
					buttonTimer = millis();
				}

			}
        free(code);		
        } 					
	}
}
/*
* ************************************************************************************
*/
void PersianaCount(void){
	checklimits();
	if(digitalRead(PERSIANA_BAJAR)==1) 
		countDcha--;		
	if(digitalRead(PERSIANA_SUBIR)==1)
		countDcha++;
}
/*
* ***********************************************************************
*/
string logTime(){
	char TimeString[128];
	timeval curTime;
	gettimeofday(&curTime, NULL);
	strftime(TimeString, 80, "%Y-%m-%d %H:%M:%S -> ", localtime(&curTime.tv_sec));
	return(TimeString);
}
/*
* ***********************************************************************
*/
void kb_handler(int s){
           LOG<<logTime()<<"Interrupcion de teclado"<<s<<endl;
		   logger.close();
           exit(1); 
}
/*
* ***********************************************************************
*/
void setupPins(){
	pinMode (PERSIANA_BAJAR, 	OUTPUT) ;
	pinMode (PERSIANA_SUBIR,	OUTPUT) ;
	pinMode (IN_PERSIANA_COUNT, INPUT) ;
	pullUpDnControl (IN_PERSIANA_COUNT, PUD_DOWN) ;
	pinMode(IN_PERSIANA_BAJAR, INPUT);
	pinMode(IN_PERSIANA_SUBIR, INPUT);
	pullUpDnControl (IN_PERSIANA_BAJAR, PUD_UP) ;
	pullUpDnControl (IN_PERSIANA_SUBIR, PUD_UP) ;
}
/*
* ***********************************************************************
*/
void cleanUp(){
	digitalWrite(PERSIANA_BAJAR , 0);
	digitalWrite(PERSIANA_SUBIR  , 0);
}
/*
* ***********************************************************************
*/
