#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include "motion.h" // generated code
#include <czmq.h>   // czmq

#define MAX_SAMPLES 1

int main(int argc, char *argv[]) 
{
    char *end_point = (argc > 1) ? argv[1] : "ipc:///tmp/dummy";
    int status;

    zctx_t *context = zctx_new ();
    void *subscriber = zsocket_new (context, ZMQ_SUB);
    zsocket_connect (subscriber, end_point);
    zsocket_set_subscribe (subscriber, ""); // set filter
    
    // Initialize DDS Section
    dds_qos_t * qos = NULL;
    dds_entity_t participant;
    dds_entity_t writer_topic;
    dds_entity_t publisher;     
    dds_entity_t writer;
    Motion_coordinate writer_msg;

    // Initialize DDS
    status = dds_init(argc, argv);
    DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

    // Create participant
    status = dds_participant_create(&participant, DDS_DOMAIN_DEFAULT, qos, NULL);
    DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

    // Create writer topic 
    status = dds_topic_create(participant, &writer_topic, &Motion_coordinate_desc, "motion", NULL, NULL);
    DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

    // Create a publisher
    status = dds_publisher_create(participant, &publisher, qos, NULL);
    DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

    // Create writer without Qos
    status = dds_writer_create(participant, &writer, writer_topic, NULL, NULL);
    DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

    printf("start motion listener\n");
    while (!zctx_interrupted) // handle ctrl+c
    { 
        zmsg_t *msg = zmsg_recv(subscriber);
        if (msg != NULL)
        {
            zframe_t *x = zmsg_pop(msg);
            zframe_t *y = zmsg_pop(msg);
            char *x_buffer = zframe_strdup(x);
            char *y_buffer = zframe_strdup(y);
            
            // assignment
            writer_msg.x = atof(x_buffer);
            writer_msg.y = atof(y_buffer);

            printf("dds write:(%.4f, %.4f)\n", writer_msg.x, writer_msg.y);

            status = dds_write(writer, &writer_msg);
            DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);
            
            // cleanup
            zframe_destroy(&x);
            zframe_destroy(&y);
            zmsg_destroy(&msg);
        }
    }

    // Clean up section
    zctx_destroy(&context);
    status = dds_instance_dispose(writer, &writer_msg);
    DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);
    dds_entity_delete(participant);
    dds_fini();

    exit(0);
}