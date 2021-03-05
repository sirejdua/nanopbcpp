#include <stdio.h>
#include <string>
#include <pb_encode.h>
#include <pb_decode.h>
#include "simple.pb.h"
#include "pb_wrapper.h"

int main()
{
    /* This is the buffer where we will store our message. */
    std::string serialized_message;
    
    /* Encode our message */
    {
        /* Allocate space on the stack to store the message data.
         *
         * Nanopb generates simple struct definitions for all the messages.
         * - check out the contents of simple.pb.h!
         * It is a good idea to always initialize your structures
         * so that you do not have garbage data from RAM in there.
         */
        SimpleMessage message = SimpleMessage_init_zero;
        
        /* Fill in the lucky number */
        message.lucky_number = 13;
        
        serialized_message = NanoPBSerialize(message, SimpleMessage_fields);
    }
    
    /* Now we could transmit the message over network, store it in a file or
     * wrap it to a pigeon's leg.
     */

    /* But because we are lazy, we will just decode it immediately. */
    
    {
        SimpleMessage message = NanoPBParse<SimpleMessage>(serialized_message, SimpleMessage_fields);
        
        /* Print the data contained in the message. */
        printf("Your lucky number was %d!\n", message.lucky_number);
    }
    
    return 0;
}

