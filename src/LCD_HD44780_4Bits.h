#ifndef LCD_HD44780_4BITS_H
#define LCD_HD44780_4BITS_H


/*============================================================================*/
/* Inclusions */
/*============================================================================*/
/* Realized interfaces */
#include "LCD_Commands.h"
#include "LCD_Special_Character_Config.h"

/* Required interfaces */
#include "Discrete_Output.h"


/*============================================================================*/
/* Types, interfaces and macros */
/*============================================================================*/
/* Macro for Configuration_Parameters */
#define LCD_CONFIG_FONT_5x8 0
#define LCD_CONFIG_FONT_5x10 1

#define LCD_CONFIG_NB_LINES_1 0
#define LCD_CONFIG_NB_LINES_2 1

#define LCD_CONFIG_NB_COLUMNS_8 0
#define LCD_CONFIG_NB_COLUMNS_16 1
#define LCD_CONFIG_NB_COLUMNS_20 2
#define LCD_CONFIG_NB_COLUMNS_32 3
#define LCD_CONFIG_NB_COLUMNS_40 4
#define LCD_CONFIG_NB_COLUMNS_80 5


/*============================================================================*/
/* Component_Type */
/*============================================================================*/
typedef struct {
    /* Command sent to chip HD44780 to set the display settings */
    /* bit carrier */
    /* b7|b6|b5|b4|b3|b2|b1|b0
        |  |  |  |  |  |  |  |
        |  |  |  |  |  |  |  --> cursor blink on/off (1/0)
        |  |  |  |  |  |  -----> cursor on/off (1/0)
        |  |  |  |  |  --------> display on/off (1/0)
        |  |  |  |  -----------> 1       
        -----------------------> not used             */
    uint8_t Display_Settings_Cmd;
} LCD_HD44780_4Bits_Var;

typedef struct {

    /* Variable attributes */
    LCD_HD44780_4Bits_Var* var_attr;

    /* Required interfaces */
    const Discrete_Output* Register_Select_Pin;
    const Discrete_Output* Enable_Pin;
    /* Data_4, Data_5, Data_6, Data_ 7 */
    const Discrete_Output* Data_Pin[4];
    const Discrete_Output* Backlight_Pin;
    
    /* Configuration_Parameters */
    uint8_t Font_Configuration:1;
    uint8_t Number_Lines_Configuration:1;
    uint8_t Number_Columns_Configuration:3;

} LCD_HD44780_4Bits;


/*============================================================================*/
/* Component_Operations */
/*============================================================================*/
void LCD4BDAct__Initialize( const LCD_HD44780_4Bits* Me );


/*============================================================================*/
/* Realized interfaces */
/*============================================================================*/
void LCD4BDAct__Commands__Set_Cursor_Position(
    const LCD_HD44780_4Bits* Me,
    uint8_t line_index,
    uint8_t column_index );
void LCD4BDAct__Commands__Print(
    const LCD_HD44780_4Bits* Me,
    char* text );
void LCD4BDAct__Commands__Print_Special_Character(
    const LCD_HD44780_4Bits* Me,
    uint8_t special_character_idx );
void LCD4BDAct__Commands__Hide_Cursor(
    const LCD_HD44780_4Bits* Me );
void LCD4BDAct__Commands__Show_Cursor(
    const LCD_HD44780_4Bits* Me );
void LCD4BDAct__Commands__Blink_Cursor(
    const LCD_HD44780_4Bits* Me );
void LCD4BDAct__Commands__No_Blink_Cursor(
    const LCD_HD44780_4Bits* Me );
void LCD4BDAct__Commands__Hide_Text(
    const LCD_HD44780_4Bits* Me );
void LCD4BDAct__Commands__Show_Text(
    const LCD_HD44780_4Bits* Me );
void LCD4BDAct__Commands__Clear_Text(
    const LCD_HD44780_4Bits* Me );
void LCD4BDAct__Commands__Turn_Off_Backlight(
    const LCD_HD44780_4Bits* Me );
void LCD4BDAct__Commands__Turn_On_Backlight(
    const LCD_HD44780_4Bits* Me );
    
void LCD4BDAct__Special_Character_Config__Create_Character(
    const LCD_HD44780_4Bits* Me,
    uint8_t special_character_id,
    uint8_t* char_map );

#endif