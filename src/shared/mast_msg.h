#ifndef MAST_MESSAGES_HPP
#define MAST_MASSAGES_HPP

#define MSG_MAST_SETGEOMETRY 45000 // format "lpii" (msg_code,&node,ship_id,model_id)
#define MSG_MAST_DELGEOMETRY 45001 // format "lp" (msg_code,&node)

#define MSG_HULL_SETGEOMETRY 45002 // format "lpii" (msg_code,&node,ship_id,model_id)

#endif
