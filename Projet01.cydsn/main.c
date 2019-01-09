#include "project.h"
#include "MyFunction.h"

typedef enum
{
    ETAT_VOIE,      ETAT_REGLAGE_VOIE,
    ETAT_SIGNAL,    ETAT_REGLAGE_SIGNAL,
    ETAT_FREQUENCE, ETAT_REGLAGE_FREQUENCE,
    ETAT_AMPLITUDE, ETAT_REGLAGE_AMPLITUDE
} etat_t;
typedef struct
{
    uint8 num;
    uint8 signal;
    uint16 Amplitude;
    uint16 Frequence;    
}Sortie;

void DEBUG_FUNCTION(void);
void Init_Modules(void);
void Init_Machine(void);
void Init_Sortie(void);
void DoState(etat_t ETAT);
void BarGraph(void);

uint8 MODEDEBUG = 1;
uint8 DEBUG_ENABLE = 0;
etat_t etat_courant;
Sortie Voie[16];
uint8 Flag_Rotary_P = 0;
uint8 Flag_Rotary_M = 0;
uint8 Flag_Rotary_B = 0;
uint8 Selection_Voie = 0;

int main(void)
{
    Init_Modules();
    Init_Sortie();
    Init_Machine();
    for (;;)
    {   
		AMux_1_Select(Voie[Selection_Voie].signal);
		AMux_2_Select(Voie[Selection_Voie].num);
        BarGraph();
        if(DEBUG_ENABLE) DEBUG_FUNCTION();
        else
        {
            DoState(etat_courant);
            switch(etat_courant)
            {
                case ETAT_VOIE:
                    if(Flag_Rotary_P==1) 
                    {
                        Flag_Rotary_P=0;
                        etat_courant = ETAT_SIGNAL;
                    }
                    if(Flag_Rotary_M==1) 
                    {
                        Flag_Rotary_M=0;
                        etat_courant = ETAT_AMPLITUDE;
                    }
                    if(Flag_Rotary_B==1) 
                    {
                        Flag_Rotary_B=0;
                        etat_courant = ETAT_REGLAGE_VOIE;
                    }
                    break;
                case ETAT_REGLAGE_VOIE:
                    if(Flag_Rotary_B==1) 
                    {
                        Flag_Rotary_B=0;
                        etat_courant = ETAT_VOIE;
                    }
                    break;
                case ETAT_SIGNAL:
                    if(Flag_Rotary_P==1) 
                    {
                        Flag_Rotary_P=0;
                        etat_courant = ETAT_FREQUENCE;
                    }
                    if(Flag_Rotary_M==1) 
                    {
                        Flag_Rotary_M=0;
                        etat_courant = ETAT_VOIE;
                    }
                    if(Flag_Rotary_B==1) 
                    {
                        Flag_Rotary_B=0;
                        etat_courant = ETAT_REGLAGE_SIGNAL;
                    }
                    break;
                case ETAT_REGLAGE_SIGNAL:
                    if(Flag_Rotary_B==1) 
                    {
                        Flag_Rotary_B=0;
                        etat_courant = ETAT_SIGNAL;
                    }
                case ETAT_FREQUENCE:
                    if(Flag_Rotary_P==1) 
                    {
                        Flag_Rotary_P=0;
                        etat_courant = ETAT_AMPLITUDE;
                    }
                    if(Flag_Rotary_M==1) 
                    {
                        Flag_Rotary_M=0;
                        etat_courant = ETAT_SIGNAL;
                    }
                    if(Flag_Rotary_B==1) 
                    {
                        Flag_Rotary_B=0;
                        etat_courant = ETAT_REGLAGE_FREQUENCE;
                    }
                    break;
                case ETAT_REGLAGE_FREQUENCE:
                    if(Flag_Rotary_B==1) 
                    {
                        Flag_Rotary_B=0;
                        etat_courant = ETAT_FREQUENCE;
                    }
                case ETAT_AMPLITUDE:
                    if(Flag_Rotary_P==1) 
                    {
                        Flag_Rotary_P=0;
                        etat_courant = ETAT_VOIE;
                    }
                    if(Flag_Rotary_M==1) 
                    {
                        Flag_Rotary_M=0;
                        etat_courant = ETAT_FREQUENCE;
                    }
                    if(Flag_Rotary_B==1) 
                    {
                        Flag_Rotary_B=0;
                        etat_courant = ETAT_REGLAGE_AMPLITUDE;
                    }
                    break;
                case ETAT_REGLAGE_AMPLITUDE:
                    if(Flag_Rotary_B==1) 
                    {
                        Flag_Rotary_B=0;
                        etat_courant = ETAT_AMPLITUDE;
                    }
                default:
                    break;
            }
        }
    }
}

void DEBUG_FUNCTION(void)
{
  //  MODEDEBUG = DEBUG_Read();
    LCD_Position(0,0);
    LCD_PrintString("DEBUG MODE :");
    LCD_Position(0,14);
    LCD_PutChar(48+MODEDEBUG);
    switch(MODEDEBUG)
    {
        case 0:
            break;
        case 1:
            //Amplitude test
            AMux_1_Select(3);
		    AMux_2_Select(0);
            while (1)
            {
                for(uint8 i = 0; i<=3; i++)
                {
                    WaveDAC8_1_SetRange(i); 
                    CyDelay(5000);
                }
            }
            break;
        default:
            break;
    }
}

void Init_Modules(void)
{
    WaveDAC8_1_Start();
    WaveDAC8_2_Start();
    //WaveDAC8_3_Start();
    //WaveDAC8_4_Start();

    AMux_1_Start();
	AMux_2_Start();
    
    isr_button_0_Start();
    isr_button_1_Start();
    isr_button_2_Start();

    LCD_Start();
}

void Init_Sortie(void)
{
    uint8 i;
    for(i=1;i>=16;i++)
    {
        Voie[i].num=i;
        Voie[i].signal=0;
        Voie[i].Amplitude=100;
        Voie[i].Frequence=600;
    }
}

void Init_Machine(void)
{
    etat_courant = ETAT_VOIE;
    CyGlobalIntEnable;
    if(DEBUG_ENABLE==0)
    {
        LED_Tension_Write(1);
        LCD_Position(0,0);
        LCD_PrintString("Bonjour :)");
        CyDelay(2000);
        LCD_ClearDisplay();
    }
}

void DoState(etat_t ETAT)
{
    CyDelay(100);
    LCD_Position(0,0);
    LCD_ClearDisplay();
    
    switch(ETAT)
    {
        case ETAT_VOIE:
            LCD_PrintString("Voie :");
            LCD_Position(0,7);
            LCD_PrintNumber(Selection_Voie);
            break;
        case ETAT_REGLAGE_VOIE:
            LCD_Position(1,0);
            LCD_PrintString("Reglage");
            LCD_Position(0,0);
            LCD_PrintString("Voie :");
            LCD_Position(0,7);
            LCD_PrintNumber(Selection_Voie);
            if(Flag_Rotary_P==1)
            {
                Flag_Rotary_P = 0;
                if(Selection_Voie == 15)Selection_Voie = 0;
                else Selection_Voie++;
            }
            if(Flag_Rotary_M==1)
            {
                Flag_Rotary_M = 0;
                if(Selection_Voie==0)Selection_Voie = 15;
                else Selection_Voie--;
            }    
            break;
        case ETAT_SIGNAL:
            LCD_PrintString("Signal :");
            LCD_Position(0,9);
            LCD_PrintNumber(Voie[Selection_Voie].signal);
            break;
        case ETAT_REGLAGE_SIGNAL:
            LCD_Position(1,0);
            LCD_PrintString("Reglage");
            LCD_Position(0,0);
            LCD_PrintString("Signal :");
            LCD_Position(0,9);
            LCD_PrintNumber(Voie[Selection_Voie].signal);
            if(Flag_Rotary_P==1)
            {
                Flag_Rotary_P = 0;
				if(Voie[Selection_Voie].signal==3)Voie[Selection_Voie].signal = 0;
                else Voie[Selection_Voie].signal++;
            }
            if(Flag_Rotary_M==1)
            {
                Flag_Rotary_M = 0;
				Flag_Rotary_P = 0;
				if(Voie[Selection_Voie].signal==0)Voie[Selection_Voie].signal = 3;
                else Voie[Selection_Voie].signal--;
            }
            break;
        case ETAT_FREQUENCE:
            LCD_PrintString("Frequence :");
            LCD_Position(0,12);
            LCD_PrintNumber(Voie[Selection_Voie].Frequence);
            break;
        case ETAT_REGLAGE_FREQUENCE:
            LCD_Position(1,0);
            LCD_PrintString("Reglage");
            LCD_Position(0,0);
            LCD_PrintString("Frequence :");
            LCD_Position(0,12);
            LCD_PrintNumber(Voie[Selection_Voie].Frequence);
            if(Flag_Rotary_P==1)
            {
                Flag_Rotary_P = 0;
                if(Voie[Selection_Voie].Frequence==1200) Voie[Selection_Voie].Frequence = 100;
                else Voie[Selection_Voie].Frequence += 100;
            }
            if(Flag_Rotary_M==1)
            {
                Flag_Rotary_M = 0;
                if(Voie[Selection_Voie].Frequence==100) Voie[Selection_Voie].Frequence = 1200;
                else Voie[Selection_Voie].Frequence -= 100;
            }
            break;
        case ETAT_AMPLITUDE:
            LCD_PrintString("Amplitude :");
            LCD_Position(0,12);
            LCD_PrintNumber(Voie[Selection_Voie].Amplitude);
            break;
        case ETAT_REGLAGE_AMPLITUDE:
            LCD_Position(1,0);
            LCD_PrintString("Reglage");
            LCD_Position(0,0);
            LCD_PrintString("Amplitude :");
            LCD_Position(0,12);
            LCD_PrintNumber(Voie[Selection_Voie].Amplitude);
            if(Flag_Rotary_P==1)
            {
                Flag_Rotary_P = 0;
                if(Voie[Selection_Voie].Amplitude==100) Voie[Selection_Voie].Amplitude = 10;
                else Voie[Selection_Voie].Amplitude += 10;
            }
            if(Flag_Rotary_M==1)
            {
                Flag_Rotary_M = 0;
                if(Voie[Selection_Voie].Amplitude==10) Voie[Selection_Voie].Amplitude = 100;
                else Voie[Selection_Voie].Amplitude -= 10;
            }
            break;
        default:
            break;            
    }
}

void BarGraph(void)
{
    switch(etat_courant)
    {
        case ETAT_VOIE:
            Control_Reg_1_Write(Voie[Selection_Voie].num);
            break;
        case ETAT_REGLAGE_VOIE:
            Control_Reg_1_Write(Voie[Selection_Voie].num);
            break;
        case ETAT_SIGNAL:
            Control_Reg_1_Write(Voie[Selection_Voie].signal);
            break;
        case ETAT_REGLAGE_SIGNAL:
            Control_Reg_1_Write(Voie[Selection_Voie].signal);
            break;
        case ETAT_AMPLITUDE:
            Control_Reg_1_Write(Voie[Selection_Voie].Amplitude/10);
            break;
        case ETAT_REGLAGE_AMPLITUDE:
            Control_Reg_1_Write(Voie[Selection_Voie].Amplitude/10);
            break;
        case ETAT_FREQUENCE:
            Control_Reg_1_Write(Voie[Selection_Voie].Frequence/100);
            break;
        case ETAT_REGLAGE_FREQUENCE:
            Control_Reg_1_Write(Voie[Selection_Voie].Frequence/100);
            break;
    }  
}