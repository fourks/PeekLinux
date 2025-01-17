/**
 * @file   kpd_virtual_key_table_def.h
 *
 * Definition of the keypad tables.
 * ***********************************************************************
 * * This file must be included only by kpd_virtual_key_table_mgt.c file.*
 * ***********************************************************************
 * Tables have to be modified according to the keypad hardwire.
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  11/02/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _KPD_VIRTUAL_KEY_TABLE_DEF_H_
#define _KPD_VIRTUAL_KEY_TABLE_DEF_H_

#ifndef _WINDOWS
   #include "board.cfg"
#endif

#include "kpd/kpd_cfg.h"


/**
 * @name Internal types
 *
 * Types used in implementation functions.
 *
 */
/*@{*/

/** Definition of the link between one virtual key and its associated ASCII value
 * according to the mode.
 * Important note: the variable virtual_ascii_table is defined at the compilation
 * as a link from virtual key Id, but for better performance, the function kpd_initialize_ascii_table
 * modify the table and and set a link from physical key Id to avoid an indirection, and so
 * losing time to search in all the table.
 */
typedef struct {  T_KPD_VIRTUAL_KEY_ID key_id;
                  char* default_value_p;
                  char* alphanumeric_value_p;
               } T_VIRTUAL_ASCII;

/** Table for definition of the link between all virtual keys and theirs associated
 * ASCII value according to the mode.
 */
typedef T_VIRTUAL_ASCII T_VIRTUAL_ASCII_TABLE[KPD_NB_PHYSICAL_KEYS];

/**
 * Definition of the link between virtual and physical key according to the mode
 * used by the client.
 */
typedef T_KPD_VIRTUAL_KEY_ID T_VPM_TABLE[KPD_NB_PHYSICAL_KEYS][KPD_NB_MODE+1];

/*@}*/



/* Definition of the static variable which define the link between physical
   and virtual keys. The position of the physical key Id in the table is
   defined by their value. if KPD_PKEY_MISC is defined to 8, its position
   in the table will be the eighth. The function kpd_vpm_table_is_valid
   check this condition.
   It's unauthorized to define two same virtual key Id in default mode
   (except KPD_KEY_NULL). No verification is done on this point. */
#if (BOARD == 7) || (BOARD == 8) || (BOARD == 9)
static T_VPM_TABLE vpm_table =
/*   Physical key Id,        Default mode,       Alphanumeric mode,  Config mode */
   {  { KPD_PKEY_0,          KPD_KEY_0,          KPD_KEY_ABC,        KPD_KEY_NULL},
      { KPD_PKEY_1,          KPD_KEY_1,          KPD_KEY_DEF,        KPD_KEY_NULL}, 
      { KPD_PKEY_2,          KPD_KEY_2,          KPD_KEY_GHI,        KPD_KEY_NULL},
      { KPD_PKEY_3,          KPD_KEY_3,          KPD_KEY_JKL,        KPD_KEY_NULL},
      { KPD_PKEY_4,          KPD_KEY_4,          KPD_KEY_MNO,        KPD_KEY_NULL},
      { KPD_PKEY_5,          KPD_KEY_5,          KPD_KEY_PQR,        KPD_KEY_NULL},
      { KPD_PKEY_6,          KPD_KEY_6,          KPD_KEY_ST,         KPD_KEY_NULL},
      { KPD_PKEY_7,          KPD_KEY_7,          KPD_KEY_UV,         KPD_KEY_NULL},
      { KPD_PKEY_8,          KPD_KEY_8,          KPD_KEY_WX,         KPD_KEY_NULL},
      { KPD_PKEY_9,          KPD_KEY_9,          KPD_KEY_YZ,         KPD_KEY_NULL},
      { KPD_PKEY_UP,         KPD_KEY_UP,         KPD_KEY_UP,         KPD_KEY_NULL},
      { KPD_PKEY_DOWN,       KPD_KEY_DOWN,       KPD_KEY_DOWN,       KPD_KEY_NULL},
      { KPD_PKEY_SOFT_LEFT,  KPD_KEY_SOFT_LEFT,  KPD_KEY_SOFT_LEFT,  KPD_KEY_NULL},
      { KPD_PKEY_SOFT_RIGHT, KPD_KEY_SOFT_RIGHT, KPD_KEY_SOFT_RIGHT, KPD_KEY_NULL},
      { KPD_PKEY_GREEN,      KPD_KEY_CONNECT,    KPD_KEY_CONNECT,    KPD_KEY_NULL},
      { KPD_PKEY_RED,        KPD_KEY_DISCONNECT, KPD_KEY_DISCONNECT, KPD_KEY_NULL},
      { KPD_PKEY_DIESE,      KPD_KEY_DIESE ,     KPD_KEY_DIESE,      KPD_KEY_NULL},
      { KPD_PKEY_STAR,       KPD_KEY_STAR,       KPD_KEY_STAR,       KPD_KEY_NULL}};
#elif (BOARD == 40) || (BOARD == 41) || (BOARD == 42) || (BOARD == 43) || (defined _WINDOWS)
static T_VPM_TABLE vpm_table =
/*   Physical key Id,        Default mode,       Alphanumeric mode,  Config mode */
   {  { KPD_PKEY_0,          KPD_KEY_0,          KPD_KEY_ABC,        KPD_KEY_NULL},
      { KPD_PKEY_1,          KPD_KEY_1,          KPD_KEY_DEF,        KPD_KEY_NULL}, 
      { KPD_PKEY_2,          KPD_KEY_2,          KPD_KEY_GHI,        KPD_KEY_NULL},
      { KPD_PKEY_3,          KPD_KEY_3,          KPD_KEY_JKL,        KPD_KEY_NULL},
      { KPD_PKEY_4,          KPD_KEY_4,          KPD_KEY_MNO,        KPD_KEY_NULL},
      { KPD_PKEY_5,          KPD_KEY_5,          KPD_KEY_PQR,        KPD_KEY_NULL},
      { KPD_PKEY_6,          KPD_KEY_6,          KPD_KEY_ST,         KPD_KEY_NULL},
      { KPD_PKEY_7,          KPD_KEY_7,          KPD_KEY_UV,         KPD_KEY_NULL},
      { KPD_PKEY_8,          KPD_KEY_8,          KPD_KEY_WX,         KPD_KEY_NULL},
      { KPD_PKEY_9,          KPD_KEY_9,          KPD_KEY_YZ,         KPD_KEY_NULL},
      { KPD_PKEY_UP,         KPD_KEY_UP,         KPD_KEY_UP,         KPD_KEY_NULL},
      { KPD_PKEY_DOWN,       KPD_KEY_DOWN,       KPD_KEY_DOWN,       KPD_KEY_NULL},
      { KPD_PKEY_SOFT_LEFT,  KPD_KEY_SOFT_LEFT,  KPD_KEY_SOFT_LEFT,  KPD_KEY_NULL},
      { KPD_PKEY_SOFT_RIGHT, KPD_KEY_SOFT_RIGHT, KPD_KEY_SOFT_RIGHT, KPD_KEY_NULL},
      { KPD_PKEY_GREEN,      KPD_KEY_CONNECT,    KPD_KEY_CONNECT,    KPD_KEY_NULL},
      { KPD_PKEY_RED,        KPD_KEY_DISCONNECT, KPD_KEY_DISCONNECT, KPD_KEY_NULL},
      { KPD_PKEY_DIESE,      KPD_KEY_DIESE ,     KPD_KEY_DIESE,      KPD_KEY_NULL},
      { KPD_PKEY_STAR,       KPD_KEY_STAR,       KPD_KEY_STAR,       KPD_KEY_NULL},
      { KPD_PKEY_VOL_UP,     KPD_KEY_VOL_UP,     KPD_KEY_VOL_UP,     KPD_KEY_NULL},
      { KPD_PKEY_VOL_DOWN,   KPD_KEY_VOL_DOWN,   KPD_KEY_VOL_DOWN,   KPD_KEY_NULL},
      { KPD_PKEY_LEFT,       KPD_KEY_LEFT,       KPD_KEY_LEFT,       KPD_KEY_NULL},
      { KPD_PKEY_RIGHT,      KPD_KEY_RIGHT,      KPD_KEY_RIGHT,      KPD_KEY_NULL},
      { KPD_PKEY_NAV_CENTER, KPD_KEY_ENTER,      KPD_KEY_ENTER,      KPD_KEY_NULL},
      { KPD_PKEY_REC,        KPD_KEY_RECORD,     KPD_KEY_RECORD,     KPD_KEY_NULL}};

#elif ((BOARD==70)||(BOARD==71))
static T_VPM_TABLE vpm_table =
/*   Physical key Id,        Default mode,       Alphanumeric mode,  Config mode */
      {{KPD_PKEY_0,           KPD_KEY_0,          KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_1,           KPD_KEY_1,          KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_2,           KPD_KEY_2,          KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_3,           KPD_KEY_3,          KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_4,           KPD_KEY_4,          KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_5,           KPD_KEY_5,          KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_6,           KPD_KEY_6,          KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_7,           KPD_KEY_7,          KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_8,           KPD_KEY_8,          KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_9,           KPD_KEY_9,          KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_A ,          KPD_KEY_A ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_B ,          KPD_KEY_B ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_C ,          KPD_KEY_C ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_D ,          KPD_KEY_D ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_E ,          KPD_KEY_E ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_F ,          KPD_KEY_F ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_G ,          KPD_KEY_G ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_H ,          KPD_KEY_H ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_I ,          KPD_KEY_I ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_J ,          KPD_KEY_J ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_K ,          KPD_KEY_K ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_L ,          KPD_KEY_L ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_M ,          KPD_KEY_M ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_N ,          KPD_KEY_N ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_O ,          KPD_KEY_O ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_P ,          KPD_KEY_P ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_Q ,          KPD_KEY_Q ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_R ,          KPD_KEY_R ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_S ,          KPD_KEY_S ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_T ,          KPD_KEY_T ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_U ,          KPD_KEY_U ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_V ,          KPD_KEY_V ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_W ,          KPD_KEY_W ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_X ,          KPD_KEY_X ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_Y ,          KPD_KEY_Y ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_Z ,          KPD_KEY_Z ,         KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_AT          ,KPD_KEY_AT         ,KPD_KEY_AT ,       KPD_KEY_NULL},
        {KPD_PKEY_SPACE       ,KPD_KEY_SPACE      ,KPD_KEY_SPACE ,       KPD_KEY_NULL},
        {KPD_PKEY_SHIFT_L,  KPD_KEY_SHIFT_L,    KPD_KEY_SHIFT_L ,       KPD_KEY_NULL},
        {KPD_PKEY_SHIFT_R       ,KPD_KEY_SHIFT_R    ,KPD_KEY_NULL ,       KPD_KEY_NULL},
        {KPD_PKEY_ENTER       ,KPD_KEY_ENTER      ,KPD_KEY_ENTER ,       KPD_KEY_NULL},
        {KPD_PKEY_LOCK        ,KPD_KEY_LOCK       ,KPD_KEY_LOCK ,       KPD_KEY_NULL},
        {KPD_PKEY_REDUCE      ,KPD_KEY_REDUCE     ,KPD_KEY_REDUCE ,       KPD_KEY_NULL},
        {KPD_PKEY_DOT         ,KPD_KEY_DOT        ,KPD_KEY_DOT ,       KPD_KEY_NULL},
        {KPD_PKEY_COMMA       ,KPD_KEY_COMMA      ,KPD_KEY_COMMA ,       KPD_KEY_NULL},
        {KPD_PKEY_QUOTE       ,KPD_KEY_QUOTE      ,KPD_KEY_QUOTE ,       KPD_KEY_NULL},
        {KPD_PKEY_NAV_CENTER  ,KPD_KEY_NAV_CENTER ,KPD_KEY_NAV_CENTER ,       KPD_KEY_NULL},
        {KPD_PKEY_CANCLE,   KPD_KEY_CANCLE,KPD_KEY_CANCLE ,       KPD_KEY_NULL},
        {KPD_PKEY_BACKSPACE   ,KPD_KEY_BACKSPACE  ,KPD_KEY_BACKSPACE ,       KPD_KEY_NULL},
        { KPD_PKEY_POWR,       KPD_KEY_POWR,       KPD_KEY_NULL,       KPD_KEY_NULL}, //zhangfanghui modify.
      { KPD_PKEY_UP,         KPD_KEY_UP,         KPD_KEY_UP,         KPD_KEY_NULL},
        { KPD_PKEY_DOWN,       KPD_KEY_DOWN,       KPD_KEY_DOWN,       KPD_KEY_NULL}};

#endif


/* Definition of the static variable which define the link between default
   virtual key and ASCII code. It's unauthorized to define two same virtual
   key Id in the table (except KPD_KEY_NULL). No verification is done on
   this point. */
#if (BOARD == 7) || (BOARD == 8) || (BOARD == 9)
static T_VIRTUAL_ASCII_TABLE virtual_ascii_table =
   {  {   KPD_KEY_0,          "0",     "+0"    },
      {   KPD_KEY_1,          "1",     " 1"    },
      {   KPD_KEY_2,          "2",     "abc2"  },
      {   KPD_KEY_3,          "3",     "def3"  },
      {   KPD_KEY_4,          "4",     "ghi4"  },
      {   KPD_KEY_5,          "5",     "jkl5"  },
      {   KPD_KEY_6,          "6",     "mno6"  },
      {   KPD_KEY_7,          "7",     "pqrs7" },
      {   KPD_KEY_8,          "8",     "tuv8"  },
      {   KPD_KEY_9,          "9",     "wxyz9" },
      {   KPD_KEY_DIESE,      "#",     ""      },
      {   KPD_KEY_STAR,       "*",     ""      },
      {   KPD_KEY_UP,         "up",    ""      },
      {   KPD_KEY_DOWN,       "down",  ""      },
      {   KPD_KEY_SOFT_LEFT,  "SoftL", ""      },
      {   KPD_KEY_SOFT_RIGHT, "SoftR", ""      },
      {   KPD_KEY_CONNECT,    "c",     ""      },
      {   KPD_KEY_DISCONNECT, "d",     ""      }};
#elif (BOARD == 40) || (BOARD == 41) || (BOARD == 42) || (BOARD == 43) || (defined _WINDOWS)
static T_VIRTUAL_ASCII_TABLE virtual_ascii_table =
   {  {   KPD_KEY_0,    "0" ,      "+0"},
      {   KPD_KEY_1,    "1",       " 1" },
      {   KPD_KEY_2,    "2"  ,     "abc2" },
      {   KPD_KEY_3,    "3"  ,     "def3" },
      {   KPD_KEY_4,    "4"  ,     "ghi4" },
      {   KPD_KEY_5,    "5"  ,     "jkl5" },
      {   KPD_KEY_6,    "6"  ,     "mno6" },
      {   KPD_KEY_7,    "7"  ,     "pqrs7" },
      {   KPD_KEY_8,    "8"  ,     "tuv8" },
      {   KPD_KEY_9,    "9"  ,     "wxyz9" },
      {   KPD_KEY_DIESE,   "#"  ,     "" },
      {   KPD_KEY_STAR,    "*"  ,     "" },
      {   KPD_KEY_UP,      "up"  ,    "" },
      {   KPD_KEY_DOWN,    "down"  ,  "" },
      {   KPD_KEY_SOFT_LEFT,  "SoftL", "" },
      {   KPD_KEY_SOFT_RIGHT, "SoftR", "" },
      {   KPD_KEY_CONNECT,    "c"  ,   "" },
      {   KPD_KEY_DISCONNECT, "d"  ,   "" },
      {   KPD_KEY_VOL_UP,     "Vol +", "" },
      {   KPD_KEY_VOL_DOWN,   "Vol -", "" },
      {   KPD_KEY_LEFT,    "left"  ,  "" },
      {   KPD_KEY_RIGHT,   "right"  , "" },
      {   KPD_KEY_ENTER,      "Enter", "" },
      {   KPD_KEY_RECORD,     "Rec"  , "" }};
#elif ((BOARD==70) || (BOARD==71))
static T_VIRTUAL_ASCII_TABLE virtual_ascii_table =

   {  {   KPD_KEY_0,    "0" ,      "+0"},
      {   KPD_KEY_1,    "1",       " 1" },
        {KPD_KEY_2,          "2" ,       "2" },
        {KPD_KEY_3,          "3" ,       "3" },
        {KPD_KEY_4,          "4" ,       "4" },
        {KPD_KEY_5,          "5" ,       "5" },
        {KPD_KEY_6,          "6" ,       "6" },
        {KPD_KEY_7,          "7" ,       "7" },
        {KPD_KEY_8,          "8" ,       "8" },
        {KPD_KEY_9,          "9" ,       "9" },
        {KPD_KEY_A ,         "A" ,       "A" },
        {KPD_KEY_B ,         "B" ,       "B" },
        {KPD_KEY_C ,         "C" ,       "C" },
        {KPD_KEY_D ,         "D" ,       "D" },
        {KPD_KEY_E ,         "E" ,       "E" },
        {KPD_KEY_F ,         "F" ,       "F" },
        {KPD_KEY_G ,         "G" ,       "G" },
        {KPD_KEY_H ,         "H" ,       "H" },
        {KPD_KEY_I ,         "I" ,       "I" },
        {KPD_KEY_J ,         "J" ,       "J" },
        {KPD_KEY_K ,         "K" ,       "K" },
        {KPD_KEY_L ,         "L" ,       "L" },
        {KPD_KEY_M ,         "M" ,       "M" },
        {KPD_KEY_N ,         "N" ,       "N" },
        {KPD_KEY_O ,         "O" ,       "O" },
        {KPD_KEY_P ,         "P" ,       "P" },
        {KPD_KEY_Q ,         "Q" ,       "Q" },
        {KPD_KEY_R ,         "R" ,       "R" },
        {KPD_KEY_S ,         "S" ,       "S" },
        {KPD_KEY_T ,         "T" ,       "T" },
        {KPD_KEY_U ,         "U" ,       "U" },
        {KPD_KEY_V ,         "V" ,       "V" },
        {KPD_KEY_W ,         "W" ,       "W" },
        {KPD_KEY_X ,         "X" ,       "X" },
        {KPD_KEY_Y ,         "Y" ,       "Y" },
        {KPD_KEY_Z ,         "Z" ,       "Z" },
        {KPD_KEY_AT         ,"AT" ,       "AT" ,       },
        {KPD_KEY_SPACE      ," " ,    "" ,    },
        {KPD_KEY_SHIFT_L,   "SHIFT" ,    "" ,    },
        {KPD_KEY_SHIFT_R,   "SHIFT" ,    "" ,    },
        {KPD_KEY_ENTER      ,"ENTER" ,    "" ,    },
        {KPD_KEY_LOCK       ,"LOCK" ,     "" ,     },
        {KPD_KEY_REDUCE     ,"-" ,   "" ,   },
        {KPD_KEY_DOT        ,"." ,      "" ,      },
        {KPD_KEY_COMMA      ,"," ,    "" ,    },
        {KPD_KEY_QUOTE      ,"QUOTE" ,    "" ,    },
        {KPD_KEY_NAV_CENTER ,"NAV_CENTER","",},
        {KPD_KEY_CANCLE     ,"CANCLE" ,   "" ,   },
        {KPD_KEY_BACKSPACE  ,"BACKSPACE" ,"" ,},
      {   KPD_KEY_UP,      "up"  ,    "" },
      {   KPD_KEY_DOWN,    "down"  ,  "" },
        {KPD_KEY_POWR,    "down"  ,  "" }};
#endif

#endif /* #ifndef _KPD_VIRTUAL_KEY_TABLE_DEF_H_ */
