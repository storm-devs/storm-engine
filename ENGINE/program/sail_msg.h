#ifndef _SAILS_MESSAGE_CODE_H_
#define _SAILS_MESSAGE_CODE_H_

    // SAILS MESSAGES
#define		MSG_SAIL_INIT		40000 // format "liil" (msg_code, shipEntity,modelEntity,bSailUp)
#define		MSG_SAIL_ROPE_TIE	40020 // format "lplpl" (msg_code, &NODE,groupNum,&position,ropenum)
#define		MSG_SAIL_ROPE_UNTIE	40021 // format "lil" (msg_code, model_id,ropenum)
#define     MSG_SAIL_TO_NEWHOST 40022 // format "liplii" (msg_code, oldModel_id,&node,groupNum, hewHost_id,newModel_id)
#define     MSG_SAIL_CHECK      40023 // format "liii" (msg_code, ship_id, newHost_id, newModel_id)
#define     MSG_SAIL_GET_PARAMS 40024 // format "lip" (msg_code, ship_id, &maxSpeed)
    // получить коэффициент максимальной скорости для корабля
#define     MSG_SAIL_GET_SPEED  40002 // format "lipf" (msg_code, ship_id,&speed, maxSpeed) //speed<-[0.f,1.f], maxSpeed=[0.f,1.f];
    // поднять/убрать паруса
//#define     MSG_SAIL_ROLL       40010 // format "lil" (msg_code, ship_id, sailSet) // sailSet=номер или ROLLTYPE_NEXT,ROLLTYPE_PREV
    // типы парусов по группам сворачивания
#define ROLLTYPE_NONE       0
#define ROLLTYPE_NORMAL     1
#define ROLLTYPE_FAST       2
//#define ROLLTYPE_NEXT       100
//#define ROLLTYPE_PREV       101
    // удалить все паруса одного корабля
#define     MSG_SAIL_DEL_GROUP  40030 // format "li" (msg_code, ship_id)
    // все паруса объекта освободить от стандартного надувания
#define     MSG_SAIL_FREE_GROUP 40032 // format "li" (msg_code, ship_id)

//=\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\=
//=/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/=
    // ROPES MESSAGES
#define		MSG_ROPE_INIT		40100 // format "lii" (msg_code, sail_id,model_id)
#define     MSG_ROPE_DELETE     40101 // format "lil" (msg_code, model_id,rope_number)
#define     MSG_ROPE_DEL_GROUP  40102 // format "li" (msg_code, model_id);

//=\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\=
//=/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/=
    // FLAGS MESSAGES
#define		MSG_FLAG_INIT		40200 // format "lil" (msg_code, model_id,nation)
#define     MSG_FLAG_DEL_GROUP  40201 // formar "li" (msg_code,model_id)
#define     MSG_FLAG_TO_NEWHOST 40210 // format "lili" (msg_code,oldmodel_id,groupNum,newmodel_id)
//=\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\=
//=/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/=
    // VANTS MESSAGES
#define     MSG_VANT_INIT       40300 // format "li" (msg_code,model_id)
#define     MSG_VANT_DEL_GROUP  40301 // format "li" (msg_code,model_id)

#endif
