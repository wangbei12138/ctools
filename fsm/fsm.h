#ifndef __FSM_H__
#define __FSM_H__
/**
 * @file fsm.h
 * Coding: utf-8
 * Author   : liushuo
 * time     : 2023/12/13 11:00
*/

#include "stdint.h"

typedef void (*STATE_ACTION)(void*);

typedef struct ACTION_MAP
{
    uint8_t 		stStateID;
    STATE_ACTION 	EnterAct;
    STATE_ACTION 	RunningAct;
    STATE_ACTION 	ExitAct;
}ACTION_MAP_t; /* 动作action表描述 */

typedef struct EVENT_MAP
{
    uint8_t	stEventID;
    uint8_t stCurState;
    uint8_t stNextState;
}EVENT_MAP_t; /* 事件event表描述 */

typedef struct FSM
{
    uint8_t u8Flag; 			/* 状态切换标志位,1表示要进行状态切换 */
    uint8_t u8EventSum;			/* 事件event总数 */
    uint8_t u8ActSum;			/* 动作action总数 */
    uint8_t stCurState;
    uint8_t stNextState;
    ACTION_MAP_t *pActionMap;
    EVENT_MAP_t *pEventMap;
    void *userData;
}FSM_t; /* 状态机控制结构 */


void fsm_init(FSM_t* pFsm,EVENT_MAP_t* pEventMap,ACTION_MAP_t *pActionMap,
    uint8_t u8EventSum,uint8_t u8ActSum,uint8_t curState);
void fsm_state_transfer(FSM_t* pFsm, uint8_t stEventID);
void action_perfrom(FSM_t* pFsm);

#endif //__FSM_H__


