#include "mikabooq.h"
#include "const.h"
#include "listx.h"

struct msg_t msgArray[MAXMSG];
struct list_head freeMSG;

struct msg_t* extractMSG();


void msgq_init(void){
	if(MAXMSG <= 0) exit();
	INIT_LIST_HEAD(&freeMSG);
	for(int i = 0; i < MAXMSG;i++){
		list_add(&(msgArray[i].m_next),&freeMSG);
	}
}


int msgq_add(struct tcb_t *sender, struct tcb_t *destination, uintptr_t value){
	int result = -1;
	if((sender != NULL && destination != NULL && !list_empty(&freeMSG))){
		struct msg_t* newMsg = extractMSG();
		newMsg->m_sender = sender;
		newMsg->m_value = value;
		list_add_tail(&(newMsg->m_next),&(destination->t_msgq)); //append the message to destination's message queue
		result = 0;
	}
	return result;

}

/*  
	Retrieves a message for the given destination, stores result in *value
	if sender == NULL: gets a message from any sender
	if sender != NULL && *sender == NULL: gets a message from any sender and stores sender tcb's address in *sender
	if sender != NULL && *sender != NULL: gets a message sent by a specific *sender
	Returns:
		0 if everything successful
		-1 if there was an error
*/
int msgq_get(struct tcb_t **sender, struct tcb_t *destination, uintptr_t *value){
	if(destination != NULL){
		struct msg_t* extractedMSG = NULL;
		if(sender == NULL){
			extractedMSG = container_of(list_next(&(destination->t_msgq)),msg_t,m_next);
		}
		else if(sender != NULL && *sender == NULL){
			extractedMSG = container_of(list_next(&(destination->t_msgq)),msg_t,m_next);
			*sender = extractedMSG->m_sender;
		}
		else if(sender != NULL && *sender != NULL){
        	struct msg_t* msgIter;
        	list_for_each_entry(msgIter,list_next(&(destination->t_msgq)),m_next) {
                if(curMsg->m_sender == *sender){
                	//if a message from given sender is found
                	extractedMSG = curMsg;
                	break;
                }
        	}
		}

		if(extractedMSG != NULL){ 
			*value = extractedMSG->m_value;
			list_del(&(extractedMSG->m_next));
		}
		else{
			//error, message not found
			return -1;
		}
	}
	else{
		//error destination==NULL
		return -1;
	}
}

struct msg_t* extractMSG(){
	struct msg_t* extractedMSG = container_of(list_next(&freeMSG),msg_t,m_next);
	list_del(&(extractedMSG->m_next));	
	return extractedMSG;
}