#include "LCD_HD44780_4Bits.h"

#include "Bits_Management.h"
#include "Microcontroller_Delay.h"


/*============================================================================*/
/* Macro */
/*============================================================================*/
#define BLINK_SETTING_BIT 0
#define CURSOR_SETTING_BIT 1
#define DISPLAY_SETTING_BIT 2

#define FONT_5x10_SETTING_BIT 2
#define TWO_LINES_SETTING_BIT 3


/*============================================================================*/
/* Attributes access */
/*============================================================================*/
#define My_Display_Settings_Cmd (Me->var_attr->Display_Settings_Cmd)


/*============================================================================*/
/* Private methods declaration */
/*============================================================================*/
static void Write_4_Bits_Data( const LCD_HD44780_4Bits* Me, uint8_t bits );
static void Write_Data( const LCD_HD44780_4Bits* Me, uint8_t data );
static void Send_Command( const LCD_HD44780_4Bits* Me, uint8_t command );
static void Send_Character( const LCD_HD44780_4Bits* Me, uint8_t character );
static void Perform_Pulse_On_Enable_Pin( const LCD_HD44780_4Bits* Me );


/*============================================================================*/
/* Component_Operations */
/*============================================================================*/
void LCD4BDAct__Initialize( const LCD_HD44780_4Bits* Me )
{
    Wait_Microsecond(15000);
    
    Me->Register_Select_Pin->Set_Level( IO_LEVEL_LOW );
    Me->Enable_Pin->Set_Level( IO_LEVEL_LOW );
    
    /* Ensure LCD is in 8 bits mode (i.e. not in 4 bits mode waiting for cmd. */
    Write_4_Bits_Data( Me, 0x03 );
    Perform_Pulse_On_Enable_Pin( Me );
    Wait_Microsecond(4100);
    Write_4_Bits_Data( Me, 0x03 );
    Perform_Pulse_On_Enable_Pin( Me );
    Wait_Microsecond(100);
    Write_4_Bits_Data( Me, 0x03 );
    Perform_Pulse_On_Enable_Pin( Me );
    
    /* Set it in 4 bits mode */
    Write_4_Bits_Data( Me, 0x02 );
    Perform_Pulse_On_Enable_Pin( Me );
    
    /* 4 bits data, number of lines, font */
    uint8_t lcd_config_cmd = 0b00100000; /* 4 bits interface */
    if( Me->Font_Configuration == LCD_CONFIG_FONT_5x10 )
    {
        Set_Bit_Uint8( lcd_config_cmd, FONT_5x10_SETTING_BIT );
    }
    if( Me->Number_Lines_Configuration == LCD_CONFIG_NB_LINES_2 )
    {
        Set_Bit_Uint8( lcd_config_cmd, TWO_LINES_SETTING_BIT );
    }
    Send_Command( Me, lcd_config_cmd );
    
    /* Text shown/hide, cursor on/off, cursor blink on/off  */
    /* Depend on component variable attribute initialization */
    Send_Command( Me, My_Display_Settings_Cmd );
    
    /* Clear display */
    LCD4BDAct__Commands__Clear_Text( Me );
    Wait_Microsecond(2000);
}


/*============================================================================*/
/* Realized interfaces */
/*============================================================================*/
void LCD4BDAct__Commands__Set_Cursor_Position(
    const LCD_HD44780_4Bits* Me,
    uint8_t line_index,
    uint8_t column_index )
{
    uint8_t nb_lines = 2;
    uint8_t nb_columns = 16;
    
    uint8_t set_DDRAM_address_cmd = 0b10000000;
    
    if( Me->Number_Lines_Configuration == LCD_CONFIG_NB_LINES_2 )
    {
        nb_lines = 2;
    }
    else
    {
        nb_lines = 1;
    }
    
    switch( Me->Number_Columns_Configuration )
    {
        case LCD_CONFIG_NB_COLUMNS_8 :
            nb_columns = 8;
            break;
        case LCD_CONFIG_NB_COLUMNS_16 :
            nb_columns = 16;
            break;
        case LCD_CONFIG_NB_COLUMNS_20 :
            nb_columns = 20;
            break;
        case LCD_CONFIG_NB_COLUMNS_32 :
            nb_columns = 32;
            break;             
        case LCD_CONFIG_NB_COLUMNS_40 :
            nb_columns = 40;
            break;
        case LCD_CONFIG_NB_COLUMNS_80 :
            nb_columns = 80;
            break;
    }

    if( line_index > nb_lines )
    {
        line_index = nb_lines;
    }
    if( column_index > nb_columns )
    {
        column_index = nb_columns;
    }
    
    if( line_index == 1 )
    {
        set_DDRAM_address_cmd |= ( column_index - 1 );
    }
    else
    { /* line_index = 2 */
        set_DDRAM_address_cmd |= ( column_index - 1 + 0x40 );
    }

    Send_Command( Me, set_DDRAM_address_cmd );
}
/*----------------------------------------------------------------------------*/
void LCD4BDAct__Commands__Print(
    const LCD_HD44780_4Bits* Me,
    char* text )
{
    uint8_t text_length = strlen( text );
    uint8_t char_idx;
    for( char_idx = 0 ; char_idx < text_length ; char_idx++ )
    {
        Send_Character( Me, text[char_idx] );
    }
}
/*----------------------------------------------------------------------------*/
void LCD4BDAct__Commands__Print_Special_Character(
    const LCD_HD44780_4Bits* Me,
    uint8_t special_character_idx )
{
    uint8_t character = special_character_idx;
    
    if( Me->Font_Configuration == LCD_CONFIG_FONT_5x10 )
    {
        if( special_character_idx > 3 )
        {
            character = 0x20; /* space */
        }
    }
    if( Me->Font_Configuration == LCD_CONFIG_FONT_5x8 )
    {
        if( special_character_idx > 7 )
        {
            character = 0x20; /* space */
        }
    }    
    Send_Character( Me, character );
}
/*----------------------------------------------------------------------------*/
void LCD4BDAct__Commands__Hide_Cursor( const LCD_HD44780_4Bits* Me )
{
    Reset_Bit_Uint8( My_Display_Settings_Cmd, CURSOR_SETTING_BIT );
    Send_Command( Me, My_Display_Settings_Cmd );
}
/*----------------------------------------------------------------------------*/
void LCD4BDAct__Commands__Show_Cursor( const LCD_HD44780_4Bits* Me )
{
    Set_Bit_Uint8( My_Display_Settings_Cmd, CURSOR_SETTING_BIT );
    Send_Command( Me, My_Display_Settings_Cmd );
}
/*----------------------------------------------------------------------------*/
void LCD4BDAct__Commands__Blink_Cursor( const LCD_HD44780_4Bits* Me )
{
    Set_Bit_Uint8( My_Display_Settings_Cmd, BLINK_SETTING_BIT );
    Send_Command( Me, My_Display_Settings_Cmd );
}
/*----------------------------------------------------------------------------*/
void LCD4BDAct__Commands__No_Blink_Cursor( const LCD_HD44780_4Bits* Me )
{
    Reset_Bit_Uint8( My_Display_Settings_Cmd, BLINK_SETTING_BIT );
    Send_Command( Me, My_Display_Settings_Cmd );
}
/*----------------------------------------------------------------------------*/
void LCD4BDAct__Commands__Hide_Text( const LCD_HD44780_4Bits* Me )
{
    Reset_Bit_Uint8( My_Display_Settings_Cmd, DISPLAY_SETTING_BIT );
    Send_Command( Me, My_Display_Settings_Cmd );
}
/*----------------------------------------------------------------------------*/
void LCD4BDAct__Commands__Show_Text( const LCD_HD44780_4Bits* Me )
{
    Set_Bit_Uint8( My_Display_Settings_Cmd, DISPLAY_SETTING_BIT );
    Send_Command( Me, My_Display_Settings_Cmd );
}
/*----------------------------------------------------------------------------*/
void LCD4BDAct__Commands__Clear_Text( const LCD_HD44780_4Bits* Me )
{
    Send_Command( Me, 0b00000001 );
}
/*----------------------------------------------------------------------------*/
void LCD4BDAct__Commands__Turn_Off_Backlight(
    const LCD_HD44780_4Bits* Me )
{
    Me->Backlight_Pin->Set_Level( IO_LEVEL_LOW );
}
/*----------------------------------------------------------------------------*/
void LCD4BDAct__Commands__Turn_On_Backlight(
    const LCD_HD44780_4Bits* Me )
{
    Me->Backlight_Pin->Set_Level( IO_LEVEL_HIGH );
}
/*----------------------------------------------------------------------------*/
void LCD4BDAct__Special_Character_Config__Create_Character(
    const LCD_HD44780_4Bits* Me,
    uint8_t special_character_id,
    uint8_t* char_map )
{
    uint8_t set_CGRAM_address_cmd = 0b01000000;
    uint8_t line_idx;
    
    if( Me->Font_Configuration == LCD_CONFIG_FONT_5x8 )
    {
        set_CGRAM_address_cmd |= (special_character_id<<3);
        Send_Command( Me, set_CGRAM_address_cmd );
        for( line_idx = 0 ; line_idx<=7 ; line_idx++ )
        {
            Send_Character( Me, char_map[line_idx] );
        }
    }
    else
    {
        /* TODO */
    }
}


/*============================================================================*/
/* Private methods definition */
/*============================================================================*/
static void Write_4_Bits_Data( const LCD_HD44780_4Bits* Me, uint8_t bits )
{
    uint8_t bit_idx;
    
    for( bit_idx=0 ; bit_idx<4 ; bit_idx++ )
    {
        if( Test_Bit_Uint8( bits, bit_idx ) )
        {
            Me->Data_Pin[bit_idx]->Set_Level( IO_LEVEL_HIGH );
        }
        else
        {
            Me->Data_Pin[bit_idx]->Set_Level( IO_LEVEL_LOW );
        }
    }
}
/*----------------------------------------------------------------------------*/
static void Write_Data( const LCD_HD44780_4Bits* Me, uint8_t data )
{
    Write_4_Bits_Data( Me, data>>4 );
    Perform_Pulse_On_Enable_Pin( Me );
    Write_4_Bits_Data( Me, data );
    Perform_Pulse_On_Enable_Pin( Me );
}
/*----------------------------------------------------------------------------*/
static void Send_Command( const LCD_HD44780_4Bits* Me, uint8_t command )
{
    Me->Register_Select_Pin->Set_Level( IO_LEVEL_LOW );
    Write_Data( Me, command );
}
/*----------------------------------------------------------------------------*/
static void Send_Character( const LCD_HD44780_4Bits* Me, uint8_t character )
{
    Me->Register_Select_Pin->Set_Level( IO_LEVEL_HIGH );
    Write_Data( Me, character );
}
/*----------------------------------------------------------------------------*/
static void Perform_Pulse_On_Enable_Pin( const LCD_HD44780_4Bits* Me )
{
    Me->Enable_Pin->Set_Level( IO_LEVEL_LOW );
    Wait_Microsecond(1);
    
    Me->Enable_Pin->Set_Level( IO_LEVEL_HIGH );
    Wait_Microsecond(1);
    
    Me->Enable_Pin->Set_Level( IO_LEVEL_LOW );
    Wait_Microsecond(1);
}