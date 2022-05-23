/*
 * File:   main.c
 * Author: RO
 *
 * Created on 7 de octubre de 2021, 11:17
 */

// DSPIC33FJ128MC802 Configuration Bit Settings

// 'C' source line config statements

// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)
#pragma config RBS = NO_RAM             // Boot Segment RAM Protection (No Boot RAM)

// FSS
#pragma config SWRP = WRPROTECT_OFF     // Secure Segment Program Write Protect (Secure segment may be written)
#pragma config SSS = NO_FLASH           // Secure Segment Program Flash Code Protection (No Secure Segment)
#pragma config RSS = NO_RAM             // Secure Segment Data RAM Protection (No Secure RAM)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = PRI              // Oscillator Mode (Primary Oscillator (XT, HS, EC))
#pragma config IESO = OFF               // Internal External Switch Over Mode (Start-up device with user-selected oscillator source)

// FOSC
#pragma config POSCMD = XT              // Primary Oscillator Source (XT Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow Only One Re-configuration)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler (1:32,768)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // POR Timer Value (128ms)
#pragma config ALTI2C = OFF             // Alternate I2C  pins (I2C mapped to SDA1/SCL1 pins)
#pragma config LPOL = ON                // Motor Control PWM Low Side Polarity bit (PWM module low side output pins have active-high output polarity)
#pragma config HPOL = ON                // Motor Control PWM High Side Polarity bit (PWM module high side output pins have active-high output polarity)
#pragma config PWMPIN = OFF             // Motor Control PWM Module Pin Mode bit (PWM module pins controlled by PWM module at device Reset)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <p33FJ128MC802.h>
#include "AuK_v1_1_6.h"
#include <stdio.h>
#include <stdbool.h>

#define numFilosofos 5
Tmutex tenedores[numFilosofos];
Tmutex pantalla;

void pensar(void)
{
    mutex_lock(&pantalla);
    printf("Filosofo %d: esta pensando\n", give_me_my_id());
    mutex_unlock(&pantalla);
}

void dormir(void)
{
    mutex_lock(&pantalla);
    printf("Filosofo %d: esta durmiendo\n", give_me_my_id());
    mutex_unlock(&pantalla);
}

void comer(void)
{
    int id = give_me_my_id();
    mutex_lock(&pantalla);
    printf("Filosofo %d: esta intentando comer\n", id);
    mutex_unlock(&pantalla);
    
    int izquierda;
    int derecha = id;
    
    if(id == 0){
        izquierda = numFilosofos - 1;
    }else{
        izquierda = id - 1;
    }
    
    if(id != 0){
        mutex_lock(&tenedores[izquierda]);
        mutex_lock(&pantalla);
        printf("Filosofo %d: obtiene el tenedor izq: %d\n", id, izquierda);
        mutex_unlock(&pantalla);
        
        mutex_lock(&tenedores[derecha]);
        mutex_lock(&pantalla);
        printf("Filosofo %d: obtiene el tenedor der: %d\n", id, derecha);
        mutex_unlock(&pantalla);
    }else{
        mutex_lock(&tenedores[derecha]);
        mutex_lock(&pantalla);
        printf("Filosofo %d: obtiene el tenedor der: %d\n", id, derecha);
        mutex_unlock(&pantalla);
       
        mutex_lock(&tenedores[izquierda]);
        mutex_lock(&pantalla);
        printf("Filosofo %d: obtiene el tenedor izq: %d\n", id, izquierda);
        mutex_unlock(&pantalla);
    }

    mutex_lock(&pantalla);
    printf("Filosofo %d: esta comiendo\n", id);
    mutex_unlock(&pantalla);
    delay_until(clock()+2);
    
    mutex_unlock(&tenedores[derecha]);
    mutex_lock(&pantalla);
    printf("Filosofo %d: libera tenedor der. %d\n", id, derecha);
    mutex_unlock(&pantalla);
    
    mutex_unlock(&tenedores[izquierda]);
    mutex_lock(&pantalla);
    printf("Filosofo %d: libera tenedor izq. %d\n", id, izquierda);
    mutex_unlock(&pantalla);
}

void run(void)
{
    while(1){
        pensar();
        comer();
        dormir();
    }
}

int main(void) 
{
    int indice;
    int pid[numFilosofos];
    
    init_task_manager();
    mutex_init(&pantalla,1);
    
    for(indice = 0; indice < numFilosofos; indice++){ //init tenedores
        mutex_init(&tenedores[indice],1);
    }
    
    for(indice = 0; indice < numFilosofos; indice++){ //init tareas
        pid[indice] = create_task((unsigned int)&run,200,1);
    }
    
    idle_task();
    return 0;
}