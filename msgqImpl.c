#include <stdint.h>
#include <libuarm.h>
#include "mikabooq.h"
#include "const.h"
#include "listx.h"

/* extracts a message from the free message list. Must check size first. */
struct msg_t* extractMSG();

//the array of messages
struct msg_t msgArray[MAXMSG];
//message free list entry point
struct list_head freeMSG;

void msgq_init(void){
    //if invalid const
    if(MAXMSG <= 0) {
        //print error message and panic
        tprint("INVALID MAXMSG\n");
        PANIC();
    }
    //init the free list
    INIT_LIST_HEAD(&freeMSG);
    //declare index
    int i;
    //for the whole array of messages
    for(i = 0; i < MAXMSG;i++){
        //add the message to the free list
        list_add(&(msgArray[i].m_next), &freeMSG);
    }
}

int msgq_add(struct tcb_t* sender, struct tcb_t* destination, uintptr_t value){
    //the result (default: error)
    int result = -1;
    //if sender and destination are not null and there is a free message available
    if((sender != NULL && destination != NULL && !list_empty(&freeMSG))){
        //extract a message
        struct msg_t* newMsg = extractMSG();
        //set the sender
        newMsg->m_sender = sender;
        //set the value
        newMsg->m_value = value;
        //enqueue the message (tail add to the list)
        list_add_tail(&(newMsg->m_next), &(destination->t_msgq));
        //add to the list of unread sent messages
        list_add(&(newMsg->m_tnext), &(sender->t_sentmsg));
        //set the result as success
        result = 0;
    }
    //return the result
    return result;
}


int msgq_get(struct tcb_t** sender, struct tcb_t* destination, uintptr_t* value){
    //the result (default: error)
    int result = -1;
    //if the given destination is not null and its message queue is not empty
    if(destination != NULL &&  !list_empty(&(destination->t_msgq))){
        //init the selected message as null
        struct msg_t* selectedMSG = NULL;
        //if sender is null (select any message)
        if(sender == NULL){
            //get the first message
            selectedMSG = container_of(list_next(&(destination->t_msgq)),struct msg_t, m_next);
        }
        //else if sender is not null but *sender is null (pointer to a null pointer)
        //(select any message and save the message sender)
        else if(sender != NULL && *sender == NULL){
            //get the first message
            selectedMSG = container_of(list_next(&(destination->t_msgq)),struct msg_t, m_next);
            //save the message sender
            *sender = selectedMSG->m_sender;
        }
        //else if a specific sender is given (pointer to a non-null pointer)
        //(select a message coming from the given sender)
        else if(sender != NULL && *sender != NULL){
            //declare the message iterator
            struct msg_t* curMessage;
            //while there is an other message and the current message sender is not
            //the given one. (implemented using the "for each entry" macro and a break)
            list_for_each_entry(curMessage,list_next(&(destination->t_msgq)), m_next) {
                //if the sender of the current message equals the given sender
                if(curMessage->m_sender == *sender){
                    //set the selected message as the current one
                    selectedMSG = curMessage;
                    //stop the loop using a break
                    break;
                }
            }
        }
        //if a message has been selected
        if(selectedMSG != NULL){
            //get and save the message value
            *value = selectedMSG->m_value;
            //free the message
            msg_free(selectedMSG);
            //set result as zero (success)
            result = 0;
        }//else leave the result as error
    }//else leave the result as error
    //return result
    return result;
}

struct msg_t* extractMSG(){
    //extract a message from the first list element
    struct msg_t* extractedMSG = container_of(list_next(&freeMSG),struct msg_t, m_next);
    //remove the extracted message from the free list
    list_del(&(extractedMSG->m_next));	
    //return the message
    return extractedMSG;
}

void msg_free(struct msg_t* msg){
    //delete the message from the queue
    list_del(&(msg->m_next));
    //delete the message from the list of sent messages in tcb
    list_del(&(msg->m_tnext));
    //add the message to the message free list
    list_add(&(msg->m_next), &freeMSG);
}