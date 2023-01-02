# ifndef __GP2X_PSP_H__
# define __GP2X_PSP_H__

# ifdef __cplusplus
extern "C" {
# endif

#include "global.h"
#include <libgame.h>

# define GP2X_CTRL_A          GE_KEY_SQUARE
# define GP2X_CTRL_B          GE_KEY_CIRCLE
# define GP2X_CTRL_Y          GE_KEY_TRIANGLE
# define GP2X_CTRL_X          GE_KEY_CROSS

# define PSP_CTRL_UP         GE_KEY_UP        
# define PSP_CTRL_RIGHT      GE_KEY_RIGHT     
# define PSP_CTRL_DOWN       GE_KEY_DOWN      
# define PSP_CTRL_LEFT       GE_KEY_LEFT      
# define PSP_CTRL_TRIANGLE   GE_KEY_TRIANGLE  
# define PSP_CTRL_CIRCLE     GE_KEY_CIRCLE    
# define PSP_CTRL_CROSS      GE_KEY_CROSS     
# define PSP_CTRL_SQUARE     GE_KEY_SQUARE    
# define PSP_CTRL_SELECT     GE_KEY_SELECT    
# define PSP_CTRL_START      GE_KEY_START     
# define PSP_CTRL_LTRIGGER   GE_KEY_L1  
# define PSP_CTRL_RTRIGGER   GE_KEY_L1  
# define PSP_CTRL_FIRE       GE_KEY_X      
# define PSP_CTRL_MASK       0xffff      


#define DELAY_KEY_FIRST 250
#define DELAY_KEY_REPEAT 80
    
  extern int  gp2xCtrlReadBufferPositive(ge_key_data_t* c, int v);
  extern int  gp2xCtrlPeekBufferPositive(ge_key_data_t* c, int v);
  extern void gp2xPowerSetClockFrequency(int freq);


  extern int  gp2xGetSoundVolume();
  extern void gp2xDecreaseVolume();
  extern void gp2xIncreaseVolume();

# ifdef __cplusplus
}
# endif

# endif
