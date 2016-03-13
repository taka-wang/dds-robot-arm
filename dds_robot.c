#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include "motion.h" // generated code
#include <rs232.h>  // librs232

#define MAX_DDS_SAMPLES 1

// global variables
static volatile int do_loop = 1;
static volatile float last_x = -140.0;
static volatile float last_y = 50.0;
static char * portname = "/dev/ttyACM0"; // default port name
static struct rs232_port_t *port = NULL;

// signal handler
static void int_handler(int dummy) 
{
    do_loop = 0;
}

// dds data arrive handler
static void data_available_handler(dds_entity_t reader) 
{
    int ret;
    dds_sample_info_t info[MAX_DDS_SAMPLES];
    void * dds_samples[MAX_DDS_SAMPLES];
    Motion_coordinate coordinate;
    uint32_t mask = DDS_ALIVE_INSTANCE_STATE;
    dds_samples[0] = &coordinate;

    unsigned int bytes = 0;
    unsigned char *direction;

    // take data
    ret = dds_take(reader, dds_samples, MAX_DDS_SAMPLES, info, mask);

    if (DDS_ERR_CHECK(ret, DDS_CHECK_REPORT)) 
    {
        printf("dds read:(%f, %f): ", coordinate.x, coordinate.y);

        if (fabs(coordinate.x - last_x) > fabs(coordinate.y - last_y) )
        {
            if (last_x > coordinate.x) 
            {
                direction = (unsigned char *)"1\n";
                printf("left\n");
            } 
            else
            {
                direction = (unsigned char *)"2\n";
                printf("right\n");
            } 
        }
        else
        {
            if (last_y > coordinate.y)
            {
                direction = (unsigned char *)"3\n";
                printf("down\n");
            }
            else
            {
                direction = (unsigned char *)"4\n";
                printf("up\n");
            }
        }

        ret = rs232_write_timeout(port, direction, 2, &bytes, 1000);

        last_x = coordinate.x;
        last_y = coordinate.y;
    }
}

static int init_serial(void)
{
    unsigned int ret = 0;
    port = rs232_init();
    if (port == NULL) return 1;

    rs232_set_device(port, portname);
    ret = rs232_open(port); // open port

    if (ret) 
    {
        printf("%s (%s)\n", rs232_strerror(ret), errno > 0 ? strerror(errno) : "");
        return ret;
    }

    rs232_set_baud(port, RS232_BAUD_115200);
    rs232_flush(port);
    // wait for arduino init
    sleep(3); 
    return 0;
}

int main(int argc, char *argv[]) 
{
    signal(SIGINT, int_handler);
    signal(SIGTERM, int_handler);

    if (argc > 1)
    {
        portname = argv[1];
    }
    
    if (init_serial() > 0) 
    {
        printf("open port:(%s) failed, are you root?\n", portname);
        exit(-1);
    }

    // Initialize DDS Section
    int status;
    dds_qos_t * qos = NULL;
    dds_entity_t participant;
    dds_entity_t reader_topic;
    dds_entity_t subscriber;
    dds_entity_t reader;
    dds_readerlistener_t listener;

    // Initialize DDS
    status = dds_init(argc, argv);
    DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

    // Create participant
    status = dds_participant_create(&participant, DDS_DOMAIN_DEFAULT, qos, NULL);
    DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

    // Create a subscriber
    status = dds_subscriber_create(participant, &subscriber, qos, NULL);
    DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);

    // Create reader topic
    status = dds_topic_create(participant, &reader_topic, &Motion_coordinate_desc, "motion", NULL, NULL);
    DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);
    
    // data available callback
    memset(&listener, 0, sizeof (listener));
    listener.on_data_available = data_available_handler;
    
    // Create reader with DATA_AVAILABLE status condition enabled
    qos = dds_qos_create();
    dds_qset_reliability(qos, DDS_RELIABILITY_BEST_EFFORT, DDS_SECS (1));
    dds_qset_history (qos, DDS_HISTORY_KEEP_ALL, 0);

    status = dds_reader_create(subscriber, &reader, reader_topic, qos, &listener);
    DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);
    dds_qos_delete (qos);

    printf("start listening\n");
    while (do_loop) 
    {
        dds_sleepfor (DDS_MSECS(10));
    }

    // Clean up section
    rs232_end(port);
    dds_status_set_enabled(reader, 0); // disable reader callback
    DDS_ERR_CHECK(status, DDS_CHECK_REPORT | DDS_CHECK_EXIT);
    dds_entity_delete(participant);
    dds_fini();

    exit(0);
}