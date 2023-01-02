/*  EnergySaver for GP2X

	File: cpulcd.c
	
	Copyright (C) 2006  Kounch
	Parts Copyright (c) 2005 Rlyeh
	Parts Copyright (c) 2006 Hermes/PS2Reality

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include <stdio.h>     //Entrada/Salida basica
//#include <sys/mman.h>  //Para poder hacer mmap y acceder a los registros de memoria
#include <unistd.h>
#include <fcntl.h>     //Para poder tener acceso al dispositivo /dev/mem y /dev/fb

#include "cpulcd.h"
#include <libgame.h>
/* Declaracion de variables globales */
#define SYS_CLK_FREQ 7372800

/*Registros del sistema*/
#ifndef __HAYMINIMAL__
 unsigned long   gp2x_dev[8]={0,0,0,0,0,0,0,0};
 volatile unsigned short *gp2x_memregs;
 volatile unsigned long  *gp2x_memregl;
#else
 extern unsigned long gp2x_dev[8];
 extern volatile unsigned short *gp2x_memregs;
 extern volatile unsigned long  *gp2x_memregl;
#endif
volatile unsigned short *MEM_REG;

static struct 
{
  unsigned short SYSCLKENREG,SYSCSETREG,FPLLVSETREG,DUALINT920,DUALINT940,DUALCTRL940;
}
system_reg;


/*Funcion que abre los dispositivos (basada en Minimal Lib)*/
void cpulcd_init()
{
  /*Inicialización para acceder a la memoria de la consola */
  //MEM_REG=&gp2x_memregs[0];
}

/*Funcion que cierra todos los dispositivos que se hayan abierto (basada en Minimal Lib)*/
int cpulcd_deinit(void)
{
//#ifndef __HAYMINIMAL__
//  if(gp2x_dev[2]) close(gp2x_dev[2]);
//#endif
  return (0);
}

/* Guardar registros de la GP2X (basado en cpuctrl de Hermes/PS2R)*/
void save_system_regs()
{
  //system_reg.SYSCSETREG=MEM_REG[0x91c>>1];
  //system_reg.FPLLVSETREG=MEM_REG[0x912>>1]; 
  //system_reg.SYSCLKENREG=MEM_REG[0x904>>1];
  //system_reg.DUALINT920=MEM_REG[0x3B40>>1];
  //system_reg.DUALINT940=MEM_REG[0x3B42>>1];  
  //system_reg.DUALCTRL940=MEM_REG[0x3B48>>1];	
}

/* Recuperar registros de la GP2X (basado en cpuctrl de Hermes/PS2R)*/
void load_system_regs()
{
  //MEM_REG[0x91c>>1]=system_reg.SYSCSETREG;
  //MEM_REG[0x910>>1]=system_reg.FPLLVSETREG;  
  //MEM_REG[0x3B40>>1]=system_reg.DUALINT920;
  //MEM_REG[0x3B42>>1]=system_reg.DUALINT940;
  //MEM_REG[0x3B48>>1]=system_reg.DUALCTRL940;
  //MEM_REG[0x904>>1]=system_reg.SYSCLKENREG;
}

/* Obtener la frecuencia del reloj (basado en cpu_speed de Hermes/PS2R)*/
unsigned get_speed_clock(void)
{
  return (GetArmCoreFreq);
}

/* Ajustar la frecuencia del reloj (basado en cpuctrl de Hermes/PS2R)*/
static int loc_gp2x_speed = 60;
void set_speed_clock(int gp2x_speed)
{
//changeARMFreq(gp2x_speed);
  return;
}

/*  (basado en cpuctrl de Hermes/PS2R)*/
unsigned get_freq_920_CLK()
{
  return (GetArmCoreFreq);
}

/*  (basado en cpuctrl de Hermes/PS2R)*/
unsigned short get_920_Div()
{
 // return (MEM_REG[0x91c>>1] & 0x7); 
}

/*  (basado en cpuctrl de Hermes/PS2R)*/
unsigned get_display_clock_div()
{
return (1);
}

/*  (basado en cpuctrl de Hermes/PS2R)*/
void set_display_clock_div(unsigned div)
{
  //return(1);
  //MEM_REG[0x924>>1]=(MEM_REG[0x924>>1] & ~(255<<8)) | div;
}

/* (basado en cpuctrl de Hermes/PS2R)*/
void set_FCLK(unsigned MHZ)
{
changeARMFreq(MHZ);
}

/* (basado en cpuctrl de Hermes/PS2R)*/
void set_DCLK_Div( unsigned short div )
{  
  //unsigned short v;
  //v = (unsigned short)( MEM_REG[0x91c>>1] & (~(0x7 << 6)) );
  //MEM_REG[0x91c>>1] = ((div & 0x7) << 6) | v; 
}

/* (basado en cpuctrl de Hermes/PS2R)*/
void set_920_Div(unsigned short div)
{
  //unsigned short v;
  //v = MEM_REG[0x91c>>1] & (~0x3);
  //MEM_REG[0x91c>>1] = (div & 0x7) | v;   
}

/* (basado en cpuctrl de Hermes/PS2R)*/
void Disable_940()
{
//Disable_Int_940();
//MEM_REG[0x3B48>>1]|= (1 << 7);
//MEM_REG[0x904>>1]&=0xfffe;
}

/* (basado en cpuctrl de Hermes/PS2R)*/
unsigned short Disable_Int_940()
{
//unsigned short ret;
//ret=MEM_REG[0x3B42>>1];
//MEM_REG[0x3B42>>1]=0;
//MEM_REG[0x3B46>>1]=0xffff;	
return 0;	
}

/* (basado en cpuctrl de Hermes/PS2R)*/
unsigned get_status_UCLK()
{
unsigned i;

//i = MEM_REG[0x900>>1];
//i = ((i >> 7) & 1) ;
//if(i) return 0;
return 1;
}

/* (basado en cpuctrl de Hermes/PS2R)*/
unsigned get_status_ACLK()
{
//unsigned i;

//i = MEM_REG[0x900>>1];
//i = ((i >> 8) & 1) ;
//if(i) return 0;

return 1;
}

/* (basado en cpuctrl de Hermes/PS2R)*/
void set_status_UCLK(unsigned s)
{
//if(s==0) MEM_REG[0x900>>1]|=128;
//else MEM_REG[0x900>>1]&=~128;
}

/* (basado en cpuctrl de Hermes/PS2R)*/
void set_status_ACLK(unsigned s)
{

//if(s==0) MEM_REG[0x900>>1]|=256;
//else MEM_REG[0x900>>1]&=~256;
}

/* Apagar o encender LCD (basado en cpuctrl de Hermes/PS2R)*/
void set_display(int mode) // 1- display on 0-display off
{
  //
  }

