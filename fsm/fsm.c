#include <stdio.h>
#include "fsm.h"

void fsm_init(FSM_t* pFsm,EVENT_MAP_t* pEventMap,ACTION_MAP_t *pActionMap,
    uint8_t u8EventSum,uint8_t u8ActSum,uint8_t curState)
{
    pFsm->u8Flag = 0;
    pFsm->stNextState = 0;
    pFsm->u8EventSum = u8EventSum;
    pFsm->u8ActSum = u8ActSum;
    pFsm->stCurState = curState;
    pFsm->pEventMap = pEventMap;
    pFsm->pActionMap = pActionMap;
}

void fsm_state_transfer(FSM_t* pFsm, uint8_t stEventID)
{
    uint8_t i = 0;
    if (pFsm == NULL) {
        printf("pFsm is null\n");
        return;
    }
    if (pFsm->pActionMap == NULL || pFsm->pEventMap == NULL) {
        printf("pFsm->pActionMap(%p), pFsm->pEventMap(%p)\n", pFsm->pActionMap, pFsm->pEventMap);
        return;
    }

    for(i=0; i<pFsm->u8EventSum; i++)
    {
        //printf("i=%d, stEventID=%d, stCurState=%d, pFsm->pEventMap[%d].stEventID=%d, pFsm->pEventMap[%d].stCurState=%d\n",
        //i, stEventID, pFsm->stCurState, i, pFsm->pEventMap[i].stEventID, i, pFsm->pEventMap[i].stCurState);
        if((stEventID == pFsm->pEventMap[i].stEventID) 
            && (pFsm->stCurState == pFsm->pEventMap[i].stCurState))
        {
            pFsm->stNextState = pFsm->pEventMap[i].stNextState;
            pFsm->u8Flag = 1;

            return;
        }
    }
}

uint8_t get_action_sum(FSM_t* pFsm,uint8_t u8ID)
{
    int i = 0;
    
    for(i=0; i<pFsm->u8ActSum; i++)
    {
        if(u8ID == pFsm->pActionMap[i].stStateID)
            return i;
    }

    return -1;
}

void action_perfrom(FSM_t* pFsm)
{
    uint8_t u8CurID = -1, u8NextID = -1;
    
    if(0 != pFsm->u8Flag)   /* 标志位不为0表示要进行状态切换 */
    {
        u8CurID = get_action_sum(pFsm,pFsm->stCurState);
        u8NextID = get_action_sum(pFsm,pFsm->stNextState);
        
        if((-1 != u8CurID) && (-1 != u8NextID))
        {
            pFsm->pActionMap[u8CurID].ExitAct(pFsm->userData);    /* 执行当前状态的退出动作 */
            pFsm->pActionMap[u8NextID].EnterAct(pFsm->userData); /* 执行下一个状态的进入动作 */
        }
        else
        {
            pFsm->u8Flag = 0;								/* 清标志位 */
            printf("State transition failed! curState = %d, nextState = %d\n",pFsm->stCurState,pFsm->stNextState);
            return;
        }
    
        pFsm->stCurState = pFsm->stNextState;			/* 当前状态切换 */
        pFsm->stNextState = -1;
        pFsm->u8Flag = 0;								/* 清标志位 */
    }
    else
    {
        u8CurID = get_action_sum(pFsm,pFsm->stCurState);
        if(-1 != u8CurID)
            pFsm->pActionMap[u8CurID].RunningAct(pFsm->userData);/* 标志位为0不进行状态切换，执行当前状态的do动作 */
    }
}
