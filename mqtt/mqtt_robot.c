#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <rs232.h>  // librs232
#include <mosquitto.h>

// MQTT 
#define MQTT_HOST   "127.0.0.1"
#define MQTT_PORT   1883
#define MQTT_Motion "/sensors/motion"

static int run = 1;
static volatile float last_x = -140.0;
static volatile float last_y = 50.0;
static char * portname = "/dev/ttyACM0"; // default port name
static struct rs232_port_t *port = NULL;

static void int_handler(int dummy)
{
    run = 0;
}

static void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
    printf("connect callback, rc=%d\n", result);
}

static void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    bool match = 0;
    int ret;
    mosquitto_topic_matches_sub(MQTT_Motion, message->topic, &match);
    float x, y;
    unsigned int bytes = 0;
    unsigned char *direction;
    
    if (match) 
    {
        sscanf((char*) message->payload, "{\"x\":%f,\"y\":%f}", &x, &y);
        printf("mqtt sub:(%.4f, %.4f): ", x, y);
        if (fabs(x - last_x) > fabs(y - last_y) )
        {
            if (last_x > x) 
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
            if (last_y > y)
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

        last_x = x;
        last_y = y;
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

    if (argc > 1) portname = argv[1];
    char * mqtt_ip  = (argc > 2) ? argv[2] : MQTT_HOST;
    int mqtt_port   = (argc > 3) ? atoi(argv[3]) : MQTT_PORT;


    if (init_serial() > 0) 
    {
        printf("open port:(%s) failed, are you root?\n", portname);
        exit(-1);
    }

    char clientid[24];
    struct mosquitto *mosq;
    int rc = 0;
    mosquitto_lib_init();

    memset(clientid, 0, 24);
    snprintf(clientid, 23, "leapmotion_robot_%d", getpid());
    mosq = mosquitto_new(clientid, true, 0);

    if (mosq) 
    {
        mosquitto_connect_callback_set(mosq, connect_callback);
        mosquitto_message_callback_set(mosq, message_callback);

        rc = mosquitto_connect(mosq, mqtt_ip, mqtt_port, 60);

        mosquitto_subscribe(mosq, NULL, MQTT_Motion, 2);

        while (run) 
        {
            rc = mosquitto_loop(mosq, -1, 1);

            if (run && rc) 
            {
                printf("connection error!\n");
                sleep(3);
                mosquitto_reconnect(mosq);
            }
        }
        sleep(1);
        mosquitto_destroy(mosq);
    }

    // cleanup
    rs232_end(port);
    mosquitto_lib_cleanup();
    return rc;
}
